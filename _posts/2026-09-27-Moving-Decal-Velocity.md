---
layout: post
title: "Preventing TAA Ghosting on Moving Decals: Writing Velocity"
categories: graphics
---

언리얼의 디퍼드 데칼은 Velocity 버퍼에 아무것도 쓰지 않는다. 그래서 캐릭터 발밑을 따라다니는 범위 표시처럼 움직이는 데칼은 TAA/TSR에서 꼬리를 끌며 번진다.

<!-- begin_excerpt -->
데칼을 AA 뒤로 빼서 그리면 번짐은 사라지지만, 이번엔 AA를 받지 못해 가장자리가 자글거린다. 그래서 데칼은 AA 앞에 그대로 두고, 대신 데칼이 자기 픽셀 단위 속도를 Velocity 버퍼에 직접 쓰게 만들어 TAA/TSR/DLSS가 데칼의 움직임을 정확히 인지하고 AA를 처리할 수 있게 하여 고스팅 현상을 없앴다.
<!-- end_excerpt -->

## 데칼 패스 순서

UE5 디퍼드 렌더러에서 한 프레임 동안 데칼과 속도가 기록되는 순서는 다음과 같다.

1. **PrePass** — 깊이를 쓴다. 설정(`r.VelocityOutputPass=0`)에 따라 여기서 속도도 쓴다.
2. **DBuffer Decals** — 베이스 패스 전에 DBufferA/B/C에 BaseColor·Normal·Roughness를 쓴다.
3. **BasePass** — GBuffer를 만들면서 DBuffer를 읽어 합성한다.
4. **Velocity** — 움직이는 불투명 지오메트리의 속도를 쓴다.
5. **GBuffer Decals / Emissive Decals** — 베이스 패스 이후에 GBuffer와 SceneColor 위에 덧그린다.
6. **Lighting**
7. **Translucency** — 반투명 속도는 여기서 따로 쓴다.
8. **TAA / TSR / DLSS** — Velocity 버퍼로 이전 프레임 히스토리를 재투영해 섞는다.
9. **Motion Blur → Tonemap**

<svg viewBox="0 0 640 470" width="100%" style="max-width:640px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif" font-size="14">
  <defs>
    <marker id="dv-arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto">
      <path d="M0,0 L10,5 L0,10 z" fill="#888"/>
    </marker>
  </defs>
  <g stroke="#888" stroke-width="1.5" fill="none">
    <rect x="20" y="10"  width="260" height="34" rx="4"/>
    <rect x="20" y="60"  width="260" height="34" rx="4"/>
    <rect x="20" y="110" width="260" height="34" rx="4"/>
    <rect x="20" y="160" width="260" height="34" rx="4"/>
    <rect x="20" y="210" width="260" height="34" rx="4"/>
    <rect x="20" y="260" width="260" height="34" rx="4" stroke="#4fc3f7" stroke-width="2.5"/>
    <rect x="20" y="310" width="260" height="34" rx="4"/>
    <rect x="20" y="360" width="260" height="34" rx="4"/>
    <rect x="20" y="410" width="260" height="34" rx="4" stroke="#ffb74d" stroke-width="2.5"/>
  </g>
  <g stroke="#888" stroke-width="1.5" marker-end="url(#dv-arrow)">
    <line x1="150" y1="44"  x2="150" y2="56"/>
    <line x1="150" y1="94"  x2="150" y2="106"/>
    <line x1="150" y1="144" x2="150" y2="156"/>
    <line x1="150" y1="194" x2="150" y2="206"/>
    <line x1="150" y1="244" x2="150" y2="256"/>
    <line x1="150" y1="294" x2="150" y2="306"/>
    <line x1="150" y1="344" x2="150" y2="356"/>
    <line x1="150" y1="394" x2="150" y2="406"/>
  </g>
  <g fill="#ccc" text-anchor="middle">
    <text x="150" y="32">PrePass</text>
    <text x="150" y="82">DBuffer Decals</text>
    <text x="150" y="132">BasePass</text>
    <text x="150" y="182">Velocity</text>
    <text x="150" y="232">GBuffer / Emissive Decals</text>
    <text x="150" y="282" fill="#4fc3f7">Moving Decal</text>
    <text x="150" y="332">Lighting</text>
    <text x="150" y="382">Translucency</text>
    <text x="150" y="432" fill="#ffb74d">TAA / TSR / DLSS</text>
  </g>
  <g font-size="13">
    <text x="300" y="32"  fill="#81c784">Depth (+ Velocity)</text>
    <text x="300" y="82"  fill="#e57373">DBuffer — Velocity 없음</text>
    <text x="300" y="132" fill="#81c784">GBuffer (+ Velocity)</text>
    <text x="300" y="182" fill="#81c784">Velocity</text>
    <text x="300" y="232" fill="#e57373">GBuffer / SceneColor — Velocity 없음</text>
    <text x="300" y="282" fill="#4fc3f7">SceneColor + Velocity + Depth</text>
    <text x="300" y="432" fill="#ffb74d">Velocity로 히스토리 재투영</text>
  </g>
</svg>

데칼이 기록되는 단계는 2번과 5번 두 곳이지만, 어느 쪽도 Velocity 버퍼를 렌더 타깃으로 잡지 않는다. 데칼 셰이더도 속도 슬롯을 0으로 채워 넘긴다.

```hlsl
// DeferredDecal.usf
DBufferData.Velocity = 0;
```

결국 데칼이 그려진 픽셀의 속도는 **데칼 아래 바닥의 속도**다. 바닥은 정지해 있으니 카메라 움직임만 남는다.

## 왜 번지는가

TAA/TSR/DLSS는 모두 현재 픽셀의 속도만큼 거슬러 올라가 이전 프레임 히스토리를 가져와 섞는다. 데칼이 A에서 B로 움직였는데 속도가 바닥 기준(0)이면 다음과 같은 일이 생긴다.

<svg viewBox="0 0 640 250" width="100%" style="max-width:640px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif" font-size="13">
  <defs>
    <marker id="rp-arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
      <path d="M0,0 L10,5 L0,10 z" fill="#ccc"/>
    </marker>
  </defs>
  <text x="20" y="22" fill="#ccc" font-size="14">Velocity = 0 (기본 데칼)</text>
  <line x1="20" y1="90" x2="300" y2="90" stroke="#888" stroke-width="2"/>
  <ellipse cx="100" cy="90" rx="45" ry="12" fill="none" stroke="#e57373" stroke-width="2" stroke-dasharray="5 4"/>
  <ellipse cx="220" cy="90" rx="45" ry="12" fill="#4fc3f7" fill-opacity="0.35" stroke="#4fc3f7" stroke-width="2"/>
  <text x="100" y="120" fill="#e57373" text-anchor="middle">A (N-1)</text>
  <text x="220" y="120" fill="#4fc3f7" text-anchor="middle">B (N)</text>
  <path d="M100,70 C100,45 100,45 100,62" fill="none" stroke="#ccc" stroke-width="1.5" marker-end="url(#rp-arrow)"/>
  <path d="M220,70 C220,45 220,45 220,62" fill="none" stroke="#ccc" stroke-width="1.5" marker-end="url(#rp-arrow)"/>
  <text x="100" y="42" fill="#ccc" text-anchor="middle">같은 자리 히스토리</text>
  <text x="220" y="42" fill="#ccc" text-anchor="middle">같은 자리 히스토리</text>
  <text x="20" y="160" fill="#e57373">A: 데칼 없는 자리에 옛 데칼이 남는다 → 꼬리</text>
  <text x="20" y="182" fill="#e57373">B: 새 데칼에 바닥 히스토리가 섞인다 → 흐려짐</text>

  <text x="340" y="22" fill="#ccc" font-size="14">Velocity = B − A (Moving Decal)</text>
  <line x1="340" y1="90" x2="620" y2="90" stroke="#888" stroke-width="2"/>
  <ellipse cx="420" cy="90" rx="45" ry="12" fill="none" stroke="#888" stroke-width="2" stroke-dasharray="5 4"/>
  <ellipse cx="540" cy="90" rx="45" ry="12" fill="#4fc3f7" fill-opacity="0.35" stroke="#4fc3f7" stroke-width="2"/>
  <text x="420" y="120" fill="#888" text-anchor="middle">A (N-1)</text>
  <text x="540" y="120" fill="#4fc3f7" text-anchor="middle">B (N)</text>
  <path d="M540,74 C520,40 440,40 424,70" fill="none" stroke="#81c784" stroke-width="1.8" marker-end="url(#rp-arrow)"/>
  <text x="480" y="36" fill="#81c784" text-anchor="middle">데칼 히스토리를 따라간다</text>
  <text x="340" y="160" fill="#81c784">B: 이전 프레임의 같은 데칼 텍셀과 섞인다</text>
  <text x="340" y="182" fill="#81c784">A: 바닥 속도 그대로 → 바닥 히스토리</text>
</svg>

**이미지 기반인 TAA/DLSS는 한계상, 히스토리에 남은 색이 원래 그 자리에 있던 지형인지 움직여 간 데칼인지 구분할 수 없다.** 히스토리 클램핑이 어느 정도 걸러내긴 하지만, 반투명하게 깔리는 데칼은 이웃 색 범위 안에 들어가기 쉬워 잔상이 그대로 통과한다. 이동 속도가 빠를수록 꼬리가 길어진다.

**DLSS도 만능은 아니다.** 추론에는 근거 데이터가 있어야 하는데, 입력으로 받는 모션 벡터가 TAA와 같으니 마찬가지로 데칼의 움직임 정보가 없다. 자체 휴리스틱으로 잘못된 히스토리를 TAA보다 잘 버리기 때문에 덜하긴 하지만, 데칼 픽셀이 움직였다는 정보 자체가 없으니 고스팅은 여전히 남는다.

## AA 뒤에 그리면?

TAA/TSR/DLSS 이후에 데칼을 그리면 재투영과 무관해지니 번짐은 사라진다. 대신 데칼 가장자리가 AA를 전혀 받지 못해 계단이 그대로 보이고, 카메라가 조금만 움직여도 서브픽셀 지터 없이 픽셀 단위로 튀며 자글거린다. 업스케일러를 쓰면 AA 이후는 출력 해상도라 렌더 해상도 깊이와 맞지 않는 문제도 생긴다.

결국 데칼은 AA 앞에 둬야 하고, 해결책은 데칼이 **자기 속도**를 Velocity 버퍼에 쓰는 것이다.

## Moving Decal 패스

기존 데칼 파이프라인을 고치지 않고, 속도를 쓰는 데칼 전용 패스를 따로 만들었다. 위 다이어그램의 파란 박스다. 기본 Velocity 패스가 끝난 뒤, Emissive 데칼 단계에서 SceneColor와 Velocity를 MRT로 같이 쓴다.

```cpp
PassParameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Resolve, ERenderTargetLoadAction::ELoad);
PassParameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.Velocity,      ERenderTargetLoadAction::ELoad);
```

### 머티리얼 그래프는 그대로

아티스트가 기존처럼 머티리얼 그래프로 데칼을 만들 수 있어야 한다. 그래서 픽셀 셰이더를 `FGlobalShader`가 아니라 `FMaterialShader`로 만들고, 전용 머티리얼 도메인(`MD_MovingDecal`)일 때만 퍼뮤테이션을 컴파일하게 했다.

```cpp
IMPLEMENT_GLOBAL_SHADER(FMovingDecalVS, "/Engine/Private/MovingDecal.usf", "MainVS", SF_Vertex);
IMPLEMENT_MATERIAL_SHADER_TYPE(, FMovingDecalPS, TEXT("/Engine/Private/MovingDecal.usf"), TEXT("MainPS"), SF_Pixel);

bool FMovingDecalPS::ShouldCompilePermutation(const FMaterialShaderPermutationParameters& Parameters)
{
	return Parameters.MaterialParameters.MaterialDomain == MD_MovingDecal;
}
```

USF에서 `/Engine/Generated/Material.ush`를 include하면 머티리얼 그래프가 번역된 코드가 그대로 들어온다. 데칼 UV를 모든 TexCoord에 넣고 `CalcPixelMaterialInputs`를 돌려 Emissive와 Opacity를 받는다. 셰이더 쪽은 속도만 책임지고, 모양·색·페이드는 전부 머티리얼 그래프가 정한다.

### 위치 복원

일반 데칼과 똑같이 단위 큐브를 래스터라이즈하고, 픽셀마다 씬 깊이로 월드 위치를 복원한 뒤 데칼 로컬 공간으로 되돌린다.

```hlsl
float  DeviceZ   = SceneDepthCopy.Load(int3(PixelPos, 0)).r;
float4 WorldPos  = float4(SvPositionToResolvedTranslatedWorld(float4(SvPosition.xy, DeviceZ, 1)), 1);
float4 DecalHom  = mul(WorldPos, TranslatedWorldToDecal);
float3 LocalPos  = DecalHom.xyz / DecalHom.w;       // -1..1 박스
float2 DecalUV   = LocalPos.xy * 0.5 + 0.5;
clip(LocalPos + 1);
clip(1 - LocalPos);
```

이 패스는 뒤에서 설명할 이유로 깊이를 쓰기 때문에, 깊이를 읽으려면 패스 직전에 씬 깊이를 복사해 SRV로 넘겨야 한다. 스텐실은 `RECEIVE_DECAL` 비트로 테스트해 "Receives Decals"를 끈 프리미티브는 건너뛴다.

### 속도 계산

핵심은 **데칼 로컬 좌표가 같은 점**이 지난 프레임엔 화면 어디에 있었는지를 구하는 것이다. 복원한 `LocalPos`를 현재 트랜스폼과 지난 프레임 트랜스폼에 각각 곱하면 된다.

```hlsl
float4 CurrClip = mul(mul(float4(LocalPos, 1), DecalToWorld),     View.TranslatedWorldToClip);
float4 PrevClip = mul(mul(float4(LocalPos, 1), PrevDecalToWorld), View.PrevTranslatedWorldToClip);

float2 CurrNDC = CurrClip.xy / CurrClip.w - View.TemporalAAJitter.xy;
float2 PrevNDC = PrevClip.xy / PrevClip.w - View.TemporalAAJitter.zw;
float  VelZ    = CurrClip.z / CurrClip.w - PrevClip.z / PrevClip.w;

OutVelocity = EncodeVelocityToTexture(float3(CurrNDC - PrevNDC, VelZ), true, true);
```

바닥 표면 위치로 속도를 구하면 다시 바닥 속도가 나온다. 데칼 로컬 좌표를 고정하고 트랜스폼만 바꿔야 "이 데칼 텍셀이 움직인 거리"가 된다. 카메라 이동은 `PrevTranslatedWorldToClip`에, 데칼 이동은 `PrevDecalToWorld`에 들어 있으니 둘이 합쳐진 최종 화면 속도가 나온다. 지터는 엔진 Velocity 셰이더와 같은 방식으로 현재·이전 프레임 값을 각각 빼 준다.

속도는 데칼이 실제로 보이는 픽셀에만 써야 한다. 그래서 머티리얼 Opacity가 0.1 미만이거나 Emissive가 0인 픽셀은 `clip`으로 버린다. 버린 픽셀은 Velocity 버퍼에 원래 바닥 속도가 그대로 남는다.

### 지난 프레임 트랜스폼

`PrevDecalToWorld`는 렌더 스레드에서 컴포넌트 ID별로 들고 있다가 매 프레임 갱신한다. 처음 보이는 프레임은 이전 값이 없으니 현재 값을 써서 데칼 이동 속도를 0으로 둔다.

```cpp
FMatrix PrevDecalToWorld = DecalToWorld;
if (FMatrix* Found = Cache->TransformHistory.Find(ComponentId))
{
	PrevDecalToWorld = *Found;
}
// ... draw ...
Cache->TransformHistory.Add(ComponentId, DecalToWorld);
```

이번 프레임에 없는 ID는 프레임 시작 시 히스토리에서 지운다. 행렬 원점은 월드 원점이 아니라 각 프레임의 `PreViewTranslation`(이전 프레임은 `PrevPreViewTranslation`)을 더한 Translated World로 넘긴다.

### 깊이를 카메라 쪽으로 당긴다

TAA와 TSR은 속도를 픽셀 하나에서 읽지 않는다. 3x3 이웃 중 **가장 가까운 깊이**를 가진 픽셀의 속도를 가져다 쓴다(velocity dilation). 데칼 픽셀은 바닥과 깊이가 똑같아서, 가장자리에서는 이웃 바닥 픽셀의 속도가 선택될 수 있다.

그래서 데칼 픽셀의 깊이를 카메라 방향으로 살짝 당겨 `SV_Depth`로 쓴다. 밝은 픽셀(Emissive 합 0.5 이상)은 5cm, 나머지는 0.001cm 당긴다.

```hlsl
float  Bias      = lerp(MinBias, MaxBias, step(0.5, dot(DecalRGB, 1)));
float3 BiasedPos = WorldPos.xyz + normalize(View.TranslatedWorldCameraOrigin - WorldPos.xyz) * Bias;
float4 BiasedClip = mul(float4(BiasedPos, 1), View.TranslatedWorldToClip);
OutDepth = BiasedClip.z / BiasedClip.w;
```

### 모션 블러는 제외

속도를 썼으니 모션 블러도 이 속도를 읽고 데칼을 번지게 만든다. 인디케이터처럼 또렷해야 하는 데칼에는 원하지 않는 결과다. 그래서 인코딩된 속도의 w 채널에 비트 하나를 추가했다. 0번 비트는 엔진이 이미 쓰는 픽셀 애니메이션(temporal responsiveness) 비트이고, 1번 비트를 모션 블러 무시 플래그로 쓴다.

```hlsl
// Common.ush — EncodeVelocityToTexture
EncodedV.w = ... | (TemporalResponsivenessMask & TEMPORAL_RESPONSIVENESS_MASK) | (uint(bIgnoreMotionBlur) << 1) ...;

// MotionBlurVelocityFlatten.usf
Velocity = DecodeIgnoreMotionBlurFromVelocityTexture(EncodedVelocity) ? 0 : DecodeVelocityFromTexture(EncodedVelocity).xy;
```

TAA/TSR/DLSS는 데칼 속도를 그대로 받아 재투영하고, 모션 블러만 그 픽셀을 정지한 것으로 본다.

### 블렌드

SceneColor는 `SrcAlpha, One` 가산 블렌드, Velocity는 블렌드 없이 덮어쓴다. 속도는 섞으면 의미가 없는 값이라 픽셀 하나에는 하나의 속도만 남아야 한다.

```cpp
GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_One, BO_Add, BF_Zero, BF_One>::GetRHI();
```

템플릿 인자는 RT0에만 적용되고, RT1(Velocity)은 기본값인 `One, Zero`, 즉 덮어쓰기가 된다.

이렇게 머티리얼 그래프는 그대로 쓰면서 데칼이 픽셀 단위로 자기 속도를 내놓으니, TAA와 DLSS가 데칼 픽셀이 이동했다는 걸 알아채고 히스토리를 올바른 자리에서 가져온다. 데칼이 빠르게 움직여도 꼬리가 남지 않고, AA 앞에서 그려지니 가장자리도 자글거리지 않는다.
