---
layout: post
title: "눈을 밟다 — 런타임 지형 변형"
categories: graphics
---

상호작용 가능한 눈(interactive snow)은 지면의 상태를 렌더 타깃(RT) 한 장에 추상화해 매 프레임 읽고 쓰는 문제로 귀결된다. 캐릭터가 눈밭을 지나가면 발자국이 파이고(carve), 밀려난 눈이 테두리에 둔덕으로 쌓이고(pile), 눈이 더 내리면 그 자국이 다시 메워진다. 여기서는 지형을 CPU에서 편집하지 않고 GPU에서 마스크를 읽어 랜드스케이프를 미는 방식으로 이걸 처음부터(0→1) 만든 과정을 기록한다. 브러시 드로우는 다대다 전투를 대비해 **인스턴스드 1드로우 + self-read**로, 지형 인지 레이는 **워커 병렬**로, 솟음은 **Nanite Tessellation** 변위로 처리했다.

<!--more-->

## 1. 눈을 밟다: 어떻게 파이고 어떻게 쌓이는가

지면의 눈 높이는 `R16F` 마스크 텍스처 한 채널에 담긴다. 아무도 밟지 않은 눈 표면을 기준값(rest) **0.75**로 두고, 이 값을 아래로 누르면 파임(carve), 위로 올리면 밟혀 밀린 둔덕(pile)이 된다. 지면 머티리얼은 이 값을 그대로 변위 높이로 읽는다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 280" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <path d="M50,112 L205,112 C232,112 236,90 252,90 C280,90 300,182 330,182 C360,182 380,90 408,90 C424,90 428,112 455,112 L610,112 L610,238 L50,238 Z"
        fill="#ffffff" fill-opacity="0.06" stroke="none"/>
  <line x1="50" y1="238" x2="610" y2="238" stroke="#8a6f57" stroke-width="2"/>
  <text x="54" y="253" fill="#a98a6d" font-size="12">지면 (랜드스케이프)</text>
  <line x1="50" y1="112" x2="610" y2="112" stroke="#9aa0a6" stroke-width="1.2" stroke-dasharray="5 5"/>
  <text x="512" y="107" fill="#c8ccd0" font-size="12">rest = 0.75</text>
  <path d="M50,112 L205,112 C232,112 236,90 252,90 C280,90 300,182 330,182 C360,182 380,90 408,90 C424,90 428,112 455,112 L610,112"
        fill="none" stroke="#7fd0ff" stroke-width="2.6"/>
  <line x1="330" y1="118" x2="330" y2="176" stroke="#ff8a80" stroke-width="1.6" marker-end="url(#ah)"/>
  <text x="342" y="150" fill="#ff8a80" font-size="12.5">carve (min)</text>
  <text x="342" y="167" fill="#ff8a80" font-size="11">바닥 = power0 × 0.75</text>
  <line x1="252" y1="106" x2="252" y2="84" stroke="#9ae6a0" stroke-width="1.6" marker-end="url(#ah2)"/>
  <text x="150" y="78" fill="#9ae6a0" font-size="12.5">pile (max) — 테두리 둔덕</text>
  <defs>
    <marker id="ah" markerWidth="8" markerHeight="8" refX="4" refY="6" orient="auto"><path d="M0,0 L4,6 L8,0 Z" fill="#ff8a80"/></marker>
    <marker id="ah2" markerWidth="8" markerHeight="8" refX="4" refY="2" orient="auto"><path d="M0,8 L4,2 L8,8 Z" fill="#9ae6a0"/></marker>
  </defs>
</svg>
<figcaption>발자국 단면. 기준선 0.75에서 중심은 파이고(carve) 테두리는 밀려 쌓인다(pile).</figcaption>
</div>
</figure>

브러시 픽셀 셰이더가 이 마스크에 발자국 하나를 새긴다. 원본 지면 머티리얼의 발자국 노드를 HLSL 글로벌 셰이더로 1:1 이식한 것이다.

``` hlsl
void MainPS(float2 UV : TEXCOORD0, nointerpolation uint InstanceID : TEXCOORD1,
            float4 SvPosition : SV_Position, out float4 OutColor : SV_Target0)
{
    FBrushInstance b = Instances[InstanceID];
    float radial = length(UV - 0.5f);                       // 브러시 중심 0 → 모서리 ~0.707

    // 월드-고정 노이즈: RT 픽셀 좌표로 두 텍스처를 서로 다른 스케일로 섞음
    float2 screenUV = SvPosition.xy / float2(MaskRes);
    float2 nbase = screenUV + b.RandomOffset;
    float pc = ParticleClouds.SampleLevel(NoiseSampler, nbase * (NoiseWorldSize / 120.0f), 0).x;
    float wn = WhiteNoise.SampleLevel   (NoiseSampler, nbase * (NoiseWorldSize / 1700.0f), 0).x;
    float noise = (lerp(pc * pc, wn * wn, 0.2f) - 0.1f) * b.CompNoise;

    // 반경+노이즈를 압축 커브로 매핑 → 프로파일 LUT → 압축계수 power0
    float r5    = (radial + noise) * 5.0f;
    float lutU  = pow(saturate(r5), b.CompCurve);
    float row   = step(0.5f, b.ProfileIndex) * 0.5f + 0.25f; // 지면 행 / 메시 행 선택
    float power0 = ProfileLUT.SampleLevel(MaskSampler, float2(lutU, row), 0).x;
    power0 = lerp(1.0f - b.MeshScale, 1.0f, power0);         // 단차만큼 파임 얕게 (§3)

    // self-read + 중심부만 부드럽게(golden-spiral blur)
    float M          = MaskTexture.SampleLevel(MaskSampler, screenUV, 0).x;
    float centerGate = saturate((1.0f - radial * 3.0f) * 5.0f);
    float Msm        = lerp(M, GoldenBlurMask(screenUV), b.SmoothHeight * centerGate);

    // ── 파임: 멱등 min ─────────────────────────────
    float target = power0 * max(Msm, MaskRest);
    float carve  = min(Msm, target);

    // ── 쌓임: 테두리 링을 rest 위로 max ────────────
    float s2      = saturate(r5 - 1.0f);
    float ridge   = 1.0f - saturate(2.0f * abs(pow(s2, 0.7f) - 0.5f));
    float piled   = max(carve, MaskRest + ridge * b.Piling * 0.3f);
    float Vnew    = (carve >= 0.74f) ? piled : carve;        // 얕으면 둔덕 / 파였으면 carve

    // 가장자리 페더 → 소스 알파(df)
    float2 edge = saturate((1.0f - 2.0f * abs(screenUV - 0.5f)) * 15.0f);
    float df    = saturate(b.DepthFraction) * edge.x * edge.y;
    OutColor = float4(Vnew, Vnew, Vnew, df);
}
```

핵심은 **carve가 `min`**이라는 것이다. 곱셈 베이스가 `max(Msm, rest)`로 항상 rest 이상으로 클램프되므로, carve의 바닥은 `power0 × rest`로 **현재값과 무관하게 고정**된다. 같은 자리를 몇 번을 밟아도 그 바닥 아래로는 안 내려간다 — 멱등(idempotent)이다. 처음엔 "밟을수록 깊어진다"고 보고 곱셈 누적으로 짰다가 값이 0으로 폭주했는데, 실제 동작은 누적이 아니라 "가장 깊은 자국 하나"를 남기는 min이었다. (이 순서무관 성질이 §4의 인스턴싱과 self-read를 가능하게 한다.)

`power0`은 발자국 **단면 모양**을 정한다. 중심(반경 0)에서 작고 가장자리로 갈수록 1에 가까워지는 곡선인데, 이 곡선을 코드에 박지 않고 **프로파일 LUT 텍스처**로 뺐다. 반경을 압축 커브로 매핑한 값으로 LUT를 샘플할 뿐이라, 아티스트가 단면 그래프를 바꾸면 셰이더 수정 없이 발자국 모양이 바뀐다. LUT의 행(`row`)을 지면/메시 두 개로 두어 같은 셰이더가 두 표면에서 다른 단면을 그린다.

`pile`은 반대로 rest 위로 올린다. `ridge`는 중심에서 조금 바깥의 링에서 최대라(테두리), 발자국을 따라 밀려 쌓인 둔덕을 만든다. `carve >= 0.74` 게이트로 이미 파인 곳엔 안 올려, 파임과 쌓임이 서로를 뭉개지 않게 가둔다.

발자국 깊이는 LUT 곡선이 정한다 — `lutU`(중심 0 → 가장자리 1)로 `power0`(0→1)를 읽고, 그게 `× 0.75`로 곧 파임 깊이가 된다. **단, pile은 이 LUT가 아니라 별도 `ridge` 항에서 나온다.** LUT는 `power0 ≤ 1`이라 `carve = power0 × 0.75 ≤ 0.75(rest)` — rest 아래로 파기만 하고 절대 위로 못 솟는다. pile은 반경으로 따로 구한 `ridge`를 rest 위에 더하는 별개 갈래다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 392" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <!-- ① LUT → carve (좌상) -->
  <text x="52" y="52" fill="#7fd0ff" font-size="13">① LUT → carve</text>
  <line x1="70" y1="78" x2="70" y2="168" stroke="#5e6b74" stroke-width="1.1"/>
  <line x1="70" y1="168" x2="252" y2="168" stroke="#5e6b74" stroke-width="1.1"/>
  <text x="58" y="82" fill="#9aa0a6" font-size="10">1</text>
  <text x="58" y="172" fill="#9aa0a6" font-size="10">0</text>
  <text x="66" y="182" fill="#9aa0a6" font-size="10">0</text>
  <text x="246" y="182" fill="#9aa0a6" font-size="10">1</text>
  <text x="118" y="182" fill="#9aa0a6" font-size="10">lutU</text>
  <text x="44" y="72" fill="#9aa0a6" font-size="10">power0</text>
  <path d="M70,149 C92,162 110,168 124,168 C160,168 196,120 232,79 C240,72 247,84 250,91" fill="none" stroke="#7fd0ff" stroke-width="2.4"/>
  <circle cx="70" cy="149" r="3" fill="#7fd0ff"/><circle cx="124" cy="168" r="3" fill="#ff8a80"/><circle cx="232" cy="79" r="3" fill="#7fd0ff"/>
  <text x="128" y="163" fill="#ff8a80" font-size="10">최심 ≈0</text>
  <text x="176" y="74" fill="#9aa0a6" font-size="10">안 파임</text>

  <!-- ② ridge → pile (우상) -->
  <text x="392" y="52" fill="#9ae6a0" font-size="13">② ridge → pile</text>
  <line x1="410" y1="78" x2="410" y2="168" stroke="#5e6b74" stroke-width="1.1"/>
  <line x1="410" y1="168" x2="592" y2="168" stroke="#5e6b74" stroke-width="1.1"/>
  <text x="398" y="82" fill="#9aa0a6" font-size="10">1</text>
  <text x="398" y="172" fill="#9aa0a6" font-size="10">0</text>
  <text x="406" y="182" fill="#9aa0a6" font-size="10">0</text>
  <text x="586" y="182" fill="#9aa0a6" font-size="10">1</text>
  <text x="470" y="182" fill="#9aa0a6" font-size="10">반경</text>
  <text x="386" y="72" fill="#9aa0a6" font-size="10">ridge</text>
  <path d="M410,166 C428,164 440,84 456,82 C486,79 516,150 592,164" fill="none" stroke="#9ae6a0" stroke-width="2.4"/>
  <circle cx="456" cy="82" r="3" fill="#9ae6a0"/>
  <text x="430" y="74" fill="#9aa0a6" font-size="10">테두리 링</text>

  <!-- 두 갈래 화살표 → 단면 -->
  <line x1="198" y1="176" x2="322" y2="342" stroke="#ff8a80" stroke-width="1.5" stroke-dasharray="5 4" marker-end="url(#mr)"/>
  <text x="150" y="250" fill="#ff8a80" font-size="11">× 0.75, min</text>
  <line x1="502" y1="176" x2="424" y2="284" stroke="#9ae6a0" stroke-width="1.5" stroke-dasharray="5 4" marker-end="url(#mg)"/>
  <text x="450" y="250" fill="#9ae6a0" font-size="11">+ rest, max</text>

  <!-- 결과 단면 -->
  <path d="M130,300 L210,300 C234,300 238,282 256,282 C292,282 320,356 335,356 C350,356 380,282 416,282 C434,282 438,300 462,300 L540,300 L540,376 L130,376 Z" fill="#ffffff" fill-opacity="0.06" stroke="none"/>
  <line x1="130" y1="376" x2="540" y2="376" stroke="#8a6f57" stroke-width="1.4" opacity="0.7"/>
  <line x1="130" y1="300" x2="540" y2="300" stroke="#9aa0a6" stroke-width="1.1" stroke-dasharray="5 5"/>
  <text x="472" y="294" fill="#c8ccd0" font-size="11">rest 0.75</text>
  <path d="M130,300 L210,300 C234,300 238,282 256,282 C292,282 320,356 335,356 C350,356 380,282 416,282 C434,282 438,300 462,300 L540,300" fill="none" stroke="#7fd0ff" stroke-width="2.4"/>
  <line x1="335" y1="306" x2="335" y2="350" stroke="#ff8a80" stroke-width="1.5" marker-end="url(#cd)"/>
  <text x="342" y="336" fill="#ff8a80" font-size="11">carve</text>
  <line x1="416" y1="298" x2="416" y2="280" stroke="#9ae6a0" stroke-width="1.5" marker-end="url(#pu)"/>
  <text x="422" y="276" fill="#9ae6a0" font-size="11">pile</text>

  <defs>
    <marker id="mr" markerWidth="9" markerHeight="9" refX="7" refY="4.5" orient="auto"><path d="M0,0 L7,4.5 L0,9 Z" fill="#ff8a80"/></marker>
    <marker id="mg" markerWidth="9" markerHeight="9" refX="7" refY="4.5" orient="auto"><path d="M0,0 L7,4.5 L0,9 Z" fill="#9ae6a0"/></marker>
    <marker id="cd" markerWidth="8" markerHeight="8" refX="4" refY="6" orient="auto"><path d="M0,0 L4,6 L8,0 Z" fill="#ff8a80"/></marker>
    <marker id="pu" markerWidth="8" markerHeight="8" refX="4" refY="2" orient="auto"><path d="M0,8 L4,2 L8,8 Z" fill="#9ae6a0"/></marker>
  </defs>
</svg>
<figcaption>두 갈래가 한 단면으로 합쳐진다. <b>LUT는 carve 전용</b>(power0 ≤ 1 → rest 아래로만 파기), <b>pile은 별도 ridge 항</b>(반경 기반, rest 위로 더함). LUT는 절대 pile을 만들지 않는다.</figcaption>
</div>
</figure>

### 발자국 단면은 왜 4차 곡선인가

이 LUT 곡선의 정체는 **4차 다항식**이다. 실제 발자국 단면에 피팅한 계수는 이렇다.

``` text
v(x) = −12.3853·x⁴ + 20.3267·x³ − 7.29197·x² − 0.00420949·x + 0.20974
```

숫자가 5개라 변수가 여러 개처럼 보이지만, 이건 **계수 5개**일 뿐이다. 항이 5개(`x⁴·x³·x²·x·x⁰`)니 계수도 5개고, **입력은 여전히 x 하나**다. 그래서 LUT도 1차원(가로 256칸, x 하나)이면 충분하다. 코드에서 세로가 2칸(256×2, `row`로 선택)인 건 변수가 2개라서가 아니라 **땅용 곡선 / 메시용 곡선 두 벌**을 나란히 저장한 것뿐 — 각각은 여전히 x 하나짜리 함수다.

그럼 왜 하필 4차인가? **차수는 곡선이 방향을 바꿀 수 있는 최대 횟수(n−1)**를 정한다. 1차(직선)는 꺾임 0번, 2차(U자) 1번, 3차(S자) 2번, 4차는 3번까지 꺾을 수 있다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 396" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <!-- 상단: 차수 사다리 -->
  <path d="M42,124 L148,66" fill="none" stroke="#8fb7c9" stroke-width="2.2"/>
  <text x="52" y="150" fill="#9aa0a6" font-size="11">1차 · 꺾임 0</text>
  <path d="M202,66 C232,138 278,138 308,66" fill="none" stroke="#8fb7c9" stroke-width="2.2"/>
  <text x="212" y="150" fill="#9aa0a6" font-size="11">2차 · 꺾임 1</text>
  <path d="M362,70 C382,70 388,124 410,124 C432,124 448,72 468,72" fill="none" stroke="#8fb7c9" stroke-width="2.2"/>
  <text x="372" y="150" fill="#9aa0a6" font-size="11">3차 · 꺾임 2</text>
  <path d="M522,80 C534,122 548,122 560,86 C572,64 588,64 600,92 C609,110 620,114 630,108" fill="none" stroke="#7fd0ff" stroke-width="2.4"/>
  <text x="520" y="150" fill="#c8ccd0" font-size="11">4차 · 꺾임 3 (우리)</text>

  <line x1="40" y1="168" x2="630" y2="168" stroke="#262c31" stroke-width="1"/>

  <!-- 하단: 실제 v(x) -->
  <text x="44" y="196" fill="#7fd0ff" font-size="12">실제 발자국 단면 v(x)</text>
  <line x1="72" y1="200" x2="72" y2="350" stroke="#5e6b74" stroke-width="1.1"/>
  <line x1="72" y1="350" x2="602" y2="350" stroke="#5e6b74" stroke-width="1.1"/>
  <text x="60" y="204" fill="#9aa0a6" font-size="10">1</text>
  <text x="60" y="354" fill="#9aa0a6" font-size="10">0</text>
  <text x="68" y="366" fill="#9aa0a6" font-size="10">0</text>
  <text x="596" y="366" fill="#9aa0a6" font-size="10">1</text>
  <text x="300" y="388" fill="#9aa0a6" font-size="11">x = lutU  (중심 0 → 가장자리 1)</text>
  <polyline points="72,318 125,327 178,341 230,350 283,346 336,327 389,295 442,255 494,219 547,201 600,222"
            fill="none" stroke="#7fd0ff" stroke-width="2.6"/>
  <circle cx="72" cy="318" r="3.2" fill="#7fd0ff"/><text x="80" y="312" fill="#c8ccd0" font-size="10.5">중심 0.21</text>
  <circle cx="230" cy="350" r="3.2" fill="#ff8a80"/><text x="176" y="340" fill="#ff8a80" font-size="10.5">가장 깊음 ≈0</text>
  <circle cx="547" cy="201" r="3.2" fill="#9ae6a0"/><text x="500" y="196" fill="#9ae6a0" font-size="10.5">가장자리 ≈1</text>
</svg>
<figcaption>차수 = 굽이의 상한(n−1). 1차 직선 → 4차는 최대 3번 꺾임. 발자국 단면(아래)은 중심 눌림(0.21) → 최심(≈0) → 복귀(≈1)라 4차라야 담긴다.</figcaption>
</div>
</figure>

발자국 단면은 "중심 살짝 눌림 → 옆이 가장 깊게 파임 → 가장자리로 복귀"라 방향 전환이 여러 번 필요하다. 직선(1차)이나 포물선 하나(2차)로는 못 담아서, 굽이 여유가 넉넉한 4차를 골라 실제 데이터에 계수를 맞춘 것이다. 단, **차수는 '굽이의 상한'일 뿐 실제로 얼마나 출렁이는지는 계수가 정한다** — 같은 4차라도 계수에 따라 거의 직선일 수도, 격하게 출렁일 수도 있다.

중심부 평활화에 쓰는 골든 스파이럴 블러는 마스크를 여러 방향으로 게더해 평균하는 작은 커널이다.

``` hlsl
float GoldenBlurMask(float2 uv)
{
    const float BlurDistance = 0.0003f; const int DistanceSteps = 2;
    const float RadialSteps = 2.0f; const float RadialOffset = 0.618f; const float TwoPi = 6.283185f;
    float stepSize = BlurDistance / DistanceSteps, curDist = 0, subOffset = 0, curColor = 0, accum = 0;
    [unroll] for (int i = 0; i < DistanceSteps; ++i) {
        curDist += stepSize;
        [unroll] for (int j = 0; j < (int)RadialSteps; ++j) {
            subOffset += 1.0f;
            float2 off = float2(cos(TwoPi*subOffset/RadialSteps), sin(TwoPi*subOffset/RadialSteps)) * curDist;
            float w = pow(curDist, 1.0f);
            curColor += MaskTexture.SampleLevel(MaskSampler, uv + off, 0).x * w; accum += w;
        }
        subOffset += RadialOffset;
    }
    return accum > 0 ? curColor / accum : MaskTexture.SampleLevel(MaskSampler, uv, 0).x;
}
```

## 2. 어떻게 솟는가 — Nanite Tessellation

값을 색으로만 칠하면 흰 페인트일 뿐이다. 실제로 지면이 솟아야 눈이다. Deformable Snow의 본질이 여기 있다 — 눈의 원래 모양을 붕괴/축적으로 바꿔야 비로소 "변형 가능한" 눈이다. 고전적으로는 Tessellation + Displacement, UE 5.0 이후 한동안 VHM(Virtual Heightfield Mesh), 콘솔 대응으로 POM(Parallax Occlusion Mapping)을 쓰기도 하지만, 여기서는 5.3 이후 다시 들어온 머티리얼 Displacement를 랜드스케이프의 **Nanite Tessellation**으로 구동한다.

Nanite가 아무리 조밀한 메시라도, 카메라를 최대로 줌인해 픽셀 레벨로 보면 폴리곤 경계가 둥글게 뭉개진다. Nanite Tessellation은 이때 **런타임에 픽셀 레벨의 마이크로 폴리곤을 생성**해 화면 기준으로 꽉 찬 지형을 만든다. 그래서 빛의 차폐·GI·Reflection 퀄리티가 확 올라가고, 무엇보다 **런타임 micro-poly 생성이라 눈을 밟으면 밟은 모양을 실제 형상으로** 만들 수 있다. 하드웨어 테셀레이션(hull/domain)과 달리 소프트웨어 래스터라이저 안에서 삼각형을 잘게 쪼개고(dicing), 쪼갠 각 도메인 포인트마다 픽셀 머티리얼을 평가해 변위를 얻어 **정점 노멀 방향으로** 민다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 250" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <line x1="60" y1="185" x2="600" y2="185" stroke="#8a6f57" stroke-width="3"/>
  <text x="60" y="205" fill="#a98a6d" font-size="12">coarse 클러스터 삼각형</text>
  <text x="60" y="221" fill="#a98a6d" font-size="12">micro-poly로 dicing → 노멀 방향 변위</text>
  <g stroke="#5e6b74" stroke-width="1"><line x1="60" y1="181" x2="60" y2="189"/><line x1="120" y1="181" x2="120" y2="189"/><line x1="180" y1="181" x2="180" y2="189"/><line x1="240" y1="181" x2="240" y2="189"/><line x1="300" y1="181" x2="300" y2="189"/><line x1="360" y1="181" x2="360" y2="189"/><line x1="420" y1="181" x2="420" y2="189"/><line x1="480" y1="181" x2="480" y2="189"/><line x1="540" y1="181" x2="540" y2="189"/><line x1="600" y1="181" x2="600" y2="189"/></g>
  <g stroke="#586169" stroke-width="1" stroke-dasharray="2 3">
    <line x1="60" y1="185" x2="60" y2="143"/><line x1="120" y1="185" x2="120" y2="137"/><line x1="180" y1="185" x2="180" y2="131"/><line x1="240" y1="185" x2="240" y2="155"/><line x1="300" y1="185" x2="300" y2="163"/><line x1="360" y1="185" x2="360" y2="155"/><line x1="420" y1="185" x2="420" y2="131"/><line x1="480" y1="185" x2="480" y2="137"/><line x1="540" y1="185" x2="540" y2="143"/>
  </g>
  <polyline points="60,143 120,137 180,131 240,155 300,163 360,155 420,131 480,137 540,143 600,143" fill="none" stroke="#7fd0ff" stroke-width="2.4"/>
  <g fill="#7fd0ff"><circle cx="60" cy="143" r="2.6"/><circle cx="120" cy="137" r="2.6"/><circle cx="180" cy="131" r="2.6"/><circle cx="240" cy="155" r="2.6"/><circle cx="300" cy="163" r="2.6"/><circle cx="360" cy="155" r="2.6"/><circle cx="420" cy="131" r="2.6"/><circle cx="480" cy="137" r="2.6"/><circle cx="540" cy="143" r="2.6"/></g>
  <text x="250" y="120" fill="#c8ccd0" font-size="12">변위된 마이크로 표면 (밟은 자국)</text>
</svg>
<figcaption>클러스터 삼각형을 마이크로 폴리곤으로 쪼개고, 각 점을 노멀 방향으로 마스크 높이만큼 민다.</figcaption>
</div>
</figure>

엔진의 도메인 포인트 평가가 그대로 노멀 방향 오프셋이다.

``` hlsl
// Engine/Shaders/Private/Nanite/NaniteRasterizationCommon.ush
float NormalizedDisplacement = GetMaterialDisplacement(PixelMaterialInputs);
float Fade = saturate(BaseClip.w * DisplacementFadeSlope + DisplacementFadeOffset);

const float Displacement = (NormalizedDisplacement - Center) * Magnitude * Fade;
float4 NormalClip = Lerp(Verts[0].NormalClip, Verts[1].NormalClip, Verts[2].NormalClip, Barycentrics).Value;
float4 PointClip  = BaseClip + NormalClip * Displacement;   // 노멀 방향으로 밀기
```

실무 세팅. 랜드스케이프 머티리얼은 Tessellation On, Magnitude 1, Center 0. 랜드스케이프 액터 스케일이 100이라 **Magnitude 1이 곧 100cm**를 뜻하고, 마스크 변위 1이 100cm가 된다. (그래프 노드가 아니라 머티리얼 Details의 `Enable Tessellation` 체크박스가 꺼져 있으면 변위를 아무리 연결해도 안 솟는다. 엔진이 요구하는 마스터 스위치다.)

변위가 노멀 방향으로 나가기 때문에, 노멀이 제각각인 일반 메시에선 표면이 벌어지거나 튄다. **조밀하고 균일한 하이트맵 그리드인 랜드스케이프와 찰떡궁합**이고, 눈·파인 지형의 물웅덩이·절벽 표현에 탁월하다. 반대로 돌 같은 딱딱한 걸 밟고 올라서는 용도는 절대 아니다.

Nanite Tessellation은 최적화가 많이 들어가 있다 — 정점 단위 중복 연산 방지, Barycentrics prebuild, 마이크로 폴리곤 단위 HZB occlusion culling, 카메라 거리 기반 dicing rate 조절 등. 그래서 "잘 쓰면 가벼운 픽셀 연산 수준"으로 볼 수 있고, 이 방식은 **가장 진보된 형태의 snow deformation**이다. 다만 카메라 1cm 오차에도 반응해 클러스터를 다시 쪼갤 만큼 민감한 물건이라, dicing rate를 낮춰 micro-poly 수를 줄여도 base pass·shadow pass·RT pass에 얹히는 부담이 무시할 수준은 아니다. 퀄리티는 확실하되 비용도 비례한다.

### 그림자·GI를 끄고, 노멀로 라이팅을 대신한다

이 방식이 "가벼운 픽셀 연산 수준"이 되는 전제가 여기 있다. 매 프레임 새로 생기는 마이크로 폴리곤이 **캐스트 그림자와 HWRT(하드웨어 레이트레이싱) GI**까지 타면 shadow pass와 RT pass 비용이 폭증한다. 변위된 마이크로 지오메트리가 만드는 그림자는 톱니가 지고 RT 픽셀의 높이 정밀도도 부족해 어차피 쓸 만하지 않다(그림자 depth-only 패스가 변위를 최소값으로 강제하는 것도 self-shadow 방지 목적이다). 그래서 눈 지형에선 **캐스트 그림자와 HWRT GI를 끈다.**

``` hlsl
// Engine/.../NaniteRasterizationCommon.ush — 그림자 패스는 변위 최소화 (self-shadow 방지)
#if DEPTH_ONLY
    if (Fade < 1.0f) { NormalizedDisplacement = 0.0f; Fade = 1.0f; }
#endif
```

문제는 그림자와 GI를 끄면 파임·둔덕의 입체감이 사라진다는 것이다. Nanite가 미는 건 위치(실루엣·시차)지 셰이딩 노멀이 아니기 때문이다. 그래서 **압축(마스크 높이) 필드에서 노멀을 재구성하고, 직접광의 디퓨즈 + 스펙큘러로 relief를 직접 만든다.** 밟혀 눌린 곳은 기울기가 가팔라 노멀이 광원 반대로 누워 어두워지고, 테두리 둔덕은 광원 쪽으로 서서 밝아진다 — 그림자 없이도 발자국이 읽히는 이유다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 300" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <line x1="40" y1="252" x2="620" y2="252" stroke="#8a6f57" stroke-width="1.5" opacity="0.7"/>
  <!-- height samples drop lines -->
  <g stroke="#586169" stroke-width="1" stroke-dasharray="2 3">
    <line x1="395" y1="135" x2="395" y2="252"/><line x1="465" y1="173" x2="465" y2="252"/>
  </g>
  <text x="372" y="248" fill="#9aa0a6" font-size="12">h&#8343;</text>
  <text x="470" y="248" fill="#9aa0a6" font-size="12">h&#7523;</text>
  <!-- snow surface -->
  <polyline points="40,216 120,214 200,182 280,132 360,120 430,150 500,196 560,214 620,216"
            fill="none" stroke="#7fd0ff" stroke-width="2.6"/>
  <!-- tangent (slope through neighbors) -->
  <line x1="372" y1="126" x2="490" y2="184" stroke="#c8ccd0" stroke-width="1.3" stroke-dasharray="6 4"/>
  <circle cx="395" cy="135" r="3.2" fill="#c8ccd0"/><circle cx="465" cy="173" r="3.2" fill="#c8ccd0"/>
  <!-- normal at P -->
  <line x1="430" y1="150" x2="463" y2="89" stroke="#9ae6a0" stroke-width="2" marker-end="url(#na)"/>
  <text x="468" y="86" fill="#9ae6a0" font-size="13">N (노멀)</text>
  <circle cx="430" cy="150" r="3.4" fill="#7fd0ff"/>
  <!-- light -->
  <line x1="560" y1="70" x2="452" y2="132" stroke="#ffd479" stroke-width="2" marker-end="url(#la)"/>
  <text x="565" y="66" fill="#ffd479" font-size="13">L (광원)</text>
  <text x="300" y="150" fill="#c8ccd0" font-size="12.5">기울기 = (h&#8343; &#8722; h&#7523;) / 2&#916;</text>
  <text x="40" y="286" fill="#c8ccd0" font-size="13">diffuse = saturate(N &#183; L)</text>
  <text x="360" y="286" fill="#c8ccd0" font-size="13">specular = (N &#183; H)&#8319;</text>
  <defs>
    <marker id="na" markerWidth="9" markerHeight="9" refX="4.5" refY="7" orient="auto"><path d="M0,0 L4.5,7 L9,0 Z" fill="#9ae6a0" transform="rotate(180 4.5 3.5)"/></marker>
    <marker id="la" markerWidth="9" markerHeight="9" refX="4.5" refY="7" orient="auto"><path d="M0,0 L4.5,7 L9,0 Z" fill="#ffd479" transform="rotate(180 4.5 3.5)"/></marker>
  </defs>
</svg>
<figcaption>노멀은 마스크 높이의 기울기다. 이웃 텍셀을 차분해 접선을 얻고, 거기 수직인 벡터가 노멀이 된다.</figcaption>
</div>
</figure>

노멀은 마스크 높이(=압축)의 그래디언트다. 이웃 텍셀을 중앙차분해 기울기를 구하고(엔진 내장 `NormalFromHeightmap`과 동형) 탄젠트 공간 노멀로 만든다.

``` hlsl
// 압축(높이) 필드 → 노멀 : 이웃 텍셀 중앙차분
float SampleH(float2 uv) { return MaskTexture.SampleLevel(MaskSampler, uv, 0).x * SnowThickness; } // R → cm

float hL = SampleH(uv - float2(Texel.x, 0));
float hR = SampleH(uv + float2(Texel.x, 0));
float hD = SampleH(uv - float2(0, Texel.y));
float hU = SampleH(uv + float2(0, Texel.y));

float2 slope = float2(hL - hR, hD - hU) / (2.0f * WorldTexelSize); // 월드 기울기(높이/거리)
float3 Nt    = normalize(float3(slope, 1.0f));                     // 탄젠트 공간 노멀
float3 N     = normalize(TransformTangentToWorld(Nt));            // 월드 셰이딩 노멀
```

> <font size="2">
> 격자무늬가 보이면 샘플 오프셋(<code>Texel</code>)을 키워 노멀이 만든 음영을 살짝 바깥으로 번지게 하면 된다.
> </font>

그림자·GI가 없으니 이 노멀로 직접광을 손으로 먹인다 — 램버트 디퓨즈 `N·L`에 스펙큘러 하이라이트를 더해 눈결의 반짝임까지 살린다.

``` hlsl
// 재구성 노멀로 직접광 계산 (그림자·GI 대체)
float3 L = normalize(-DirectionalLightDir);
float3 V = normalize(CameraPosition - WorldPosition);
float3 H = normalize(L + V);

float  NdotL   = saturate(dot(N, L));
float3 diffuse = SnowAlbedo * LightColor * NdotL;                     // 램버트 디퓨즈
float  spec    = pow(saturate(dot(N, H)), SpecPower) * SpecIntensity; // 블린-퐁 스펙큘러
float3 shaded  = diffuse + LightColor * spec;
```

가까이선 변위가 실제 실루엣을 만들고, 멀어서 변위가 페이드로 죽어도 이 노멀 기반 디퓨즈+스펙큘러가 음영을 남긴다. 그림자 한 장, 레이 한 번 안 쏘고 파임·둔덕이 빛에 반응하는 이유다.

## 3. 지면 인지: 어디를 밟았나

발자국을 찍으려면 매 프레임 각 캐릭터의 발밑이 지면 어디에 닿는지 알아야 한다. 캐릭터가 많아지면 이 레이도 그만큼 많아진다. 렌더·게임 스레드는 이미 프레임 부하의 대부분을 지므로, **서로 독립적인 레이는 워커로 흩뿌리는 게 거의 항상 이득**이다. 발자국 컴포넌트들의 트레이스를 `ParallelFor`로 돌리고, 결과 브러시는 미리 크기를 잡아둔 버퍼에 원자적 카운터로 락 없이 채운다.

``` cpp
BrushBuffer.SetNumZeroed(Num * 2);          // 발자국 하나 = 점 + 트레일 최대 2개
FThreadSafeCounter BrushCount;

auto ProcessOne = [&](int32 i)
{
    UFootprintComponent* Comp = Interactions[i];
    // (게이팅) 거리 컬링 / 샘플 레이트 스로틀 / 최소 이동
    if (Dist(Current, ControlPoint) > Comp->GetActiveDistance()) return;
    if ((Comp->SampleAccum += DeltaTime) < 1.f / Comp->GetMaxSampleRate()) return;
    Comp->SampleAccum = 0.f;
    if (Comp->bHasLast && Dist2D(Current, Comp->LastTraceLocation) < MinSampleDistance) return;

    // 지면 트레이스
    FHitResult Hit;
    if (!GroundTrace(World, Current + FVector(0,0,TraceStartUp), Current - FVector(0,0,TraceDown), ..., Hit))
        return;

    // ... 단차 보정 → 침투 깊이 → 브러시 push (아래)
    const int32 Idx = BrushCount.Add(1);
    BrushBuffer[Idx] = MakeBrushInstance(...);
};

if (bDebug) { for (int32 i = 0; i < Num; ++i) ProcessOne(i); } // 디버그 드로우는 순차 (DrawDebug 비스레드세이프)
else        { ParallelFor(Num, ProcessOne); }                  // 평상시 병렬
BrushBuffer.SetNum(BrushCount.GetValue(), EAllowShrinking::No);
```

병렬화 이전에 쏘는 레이 수부터 줄이는 게 먼저다. 컨트롤 포인트(플레이어/카메라)에서 활성 거리를 넘으면 컬링, `1/MaxSampleRate` 간격으로만 샘플, 직전 위치에서 최소 이동 거리 미만이면 스킵 — 원거리·정지·미세이동을 통째로 끊고 남은 레이만 병렬로 돌린다.

관점을 바꾸면 이 병렬화가 왜 거의 공짜인지 분명해진다. 한 프레임의 시간 예산은 대부분 **렌더 스레드와 게임(메인) 스레드**가 잡아먹는다(대략 90%). 나머지 코어의 워커 스레드는 상대적으로 논다. 발밑 라인 트레이스는 발끼리 의존이 없어 완벽히 독립적이라(embarrassingly parallel), `ParallelFor`가 이걸 그 노는 워커들에 흩뿌린다. 그래서 **수백 마리가 초당 60번(=매 프레임 한 번씩) 밟아도, 그 비용은 크리티컬 패스인 렌더/메인이 아니라 워커에 실린다.** 메인 스레드에 남는 건 디스패치와 조인 정도의 얇은 비용뿐이다.

<figure>
<div style="text-align:center;">
<svg viewBox="0 0 660 232" width="100%" style="max-width:660px" xmlns="http://www.w3.org/2000/svg" font-family="sans-serif">
  <text x="20" y="20" fill="#c8ccd0" font-size="12.5">한 프레임의 스레드 점유 (개념도)</text>

  <text x="20" y="55" fill="#9aa0a6" font-size="11">렌더</text>
  <rect x="120" y="43" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="43" width="431" height="16" rx="4" fill="#ff8a80" opacity="0.8"/>
  <text x="20" y="83" fill="#9aa0a6" font-size="11">메인</text>
  <rect x="120" y="71" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="71" width="412" height="16" rx="4" fill="#ff8a80" opacity="0.8"/>
  <text x="556" y="69" fill="#ff8a80" font-size="10.5">≈ 90%</text>

  <line x1="20" y1="99" x2="620" y2="99" stroke="#262c31" stroke-width="1"/>

  <text x="20" y="123" fill="#9aa0a6" font-size="11">워커 0</text>
  <rect x="120" y="111" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="111" width="108" height="16" rx="4" fill="#9ae6a0" opacity="0.8"/>
  <text x="20" y="151" fill="#9aa0a6" font-size="11">워커 1</text>
  <rect x="120" y="139" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="139" width="127" height="16" rx="4" fill="#9ae6a0" opacity="0.8"/>
  <text x="20" y="179" fill="#9aa0a6" font-size="11">워커 2</text>
  <rect x="120" y="167" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="167" width="88" height="16" rx="4" fill="#9ae6a0" opacity="0.8"/>
  <text x="20" y="207" fill="#9aa0a6" font-size="11">워커 3</text>
  <rect x="120" y="195" width="490" height="16" rx="4" fill="#16191c" stroke="#262c31"/>
  <rect x="120" y="195" width="118" height="16" rx="4" fill="#9ae6a0" opacity="0.8"/>
  <text x="330" y="151" fill="#9ae6a0" font-size="10.5">← 발 트레이스가 여기로 분산</text>
</svg>
<figcaption>렌더·메인이 프레임 시간의 대부분을 잡는다. 서로 독립인 발밑 트레이스는 <code>ParallelFor</code>가 노는 워커 코어로 흩뿌려, 수백 마리가 초당 60회 밟아도 크리티컬 패스는 거의 그대로다.</figcaption>
</div>
</figure>

수치로 보면 발 N개가 매 프레임 1회씩이면 프레임당 최대 N개의 트레이스지만, 게이팅이 원거리·정지·미세이동을 먼저 걷어내고 남은 것만 워커 K개에 나뉘어 프레임당 대략 N/K로 준다. 물리 씬 쿼리는 읽기라 병렬 실행이 안전하고(각 태스크가 자기 `FHitResult`·`QueryParams`를 로컬로 가짐), 유일한 공유 쓰기인 브러시 버퍼는 락 없이 `FThreadSafeCounter`로 채운다. 결국 "수백 마리가 눈밭을 밟는" 최악의 순간에도 프레임 타임을 좌우하는 두 스레드는 거의 건드리지 않는다.

### 눈 위에 얹은 것 — 단차만큼 얕게

지면이 항상 순수 랜드스케이프는 아니다. 밋밋함을 깨려고 랜드스케이프 위에 눈 지형 메시, 데칼, 프랍을 얹어 variation을 준다. 문제는 이렇게 얹은 것이 지면보다 떠 있어(단차), 그 위를 밟으면 발자국이 실제 눈 층보다 얕게 파여야 한다는 것이다. 그래서 발이 랜드스케이프가 아닌 것에 닿으면 한 번 더 아래로 트레이스해 진짜 지면을 찾고, 뜬 높이만큼 남은 눈 깊이를 깎는다.

``` cpp
bool ComputeMeshPenScale(const FHitResult& SurfaceHit, float SnowDepth, ..., float& OutMeshPenScale)
{
    OutMeshPenScale = 1.f;
    if (Cast<ALandscapeProxy>(SurfaceHit.GetActor())) return true;   // 랜드스케이프면 그대로

    FHitResult LandHit;                                              // 메시 아래 진짜 지면
    GroundTrace(World, SurfaceHit.ImpactPoint + FVector(0,0,1),
                       SurfaceHit.ImpactPoint - FVector(0,0,TraceDown), ..., LandHit);

    const float DeltaMesh   = SurfaceHit.ImpactPoint.Z - LandHit.ImpactPoint.Z; // 메시가 뜬 높이(단차)
    const float PokeThrough = SnowDepth - DeltaMesh;                            // 남은 눈 깊이
    if (PokeThrough <= 0.f) return false;                                       // 눈보다 두꺼운 것 → 발자국 없음
    OutMeshPenScale = saturate(PokeThrough / SnowDepth);                        // 파임 깊이 스케일
    return true;
}
```

이 `MeshPenScale`이 브러시에 실려 셰이더의 `power0 = lerp(1-MeshScale, 1, power0)`로 단차 큰 자리를 덜 파이게 만든다. 또 메시 위/지면 위를 나누는 `bOnMesh`가 브러시 크기·스트레치와 프로파일 LUT 행을 바꿔 두 표면의 단면을 다르게 그린다. 눈 위의 데칼과 발자국을 분리하는 것도 같은 맥락의 이슈였다.

### 침투 깊이와 트레일 연속화

발이 눈에 얼마나 잠겼는지(`Lift`를 최대 침투 `MaxLift`로 정규화)로 발자국 깊이를 정한다. 이동 속도로 침투 커브의 샤프니스를 블렌드해, 달릴 때와 걸을 때 자국의 또렷함이 달라진다.

``` cpp
float Pen = saturate(1.f - Lift / MaxLift);
const float EffectivePenCurve = FMath::Lerp(PenCurveSlow, PenCurve, SpeedAlpha); // 속도로 커브 블렌드
Pen = FMath::Pow(Pen, FMath::Lerp(EffectivePenCurve, 1.f, CurveLerp));
Pen *= RandStream.FRandRange(1.f - TrailRandomness, 1.f);                          // 약간의 랜덤

AddBrush(Impact, 0.f, MoveYaw);                          // 점 발자국
if (bContinuous) {                                       // 직전에도 지면을 맞혔으면
    const float Gap = Dist2D(From, Impact);
    if (Gap > 0 && Gap <= MaxTrailGap)                   // 너무 벌어지지 않았으면
        AddBrush(Lerp(Impact, From, 0.5f), Gap, MoveYaw);// 중점에 길이 Gap짜리 스트레치 브러시
}
```

이산 트레이스지만 캐릭터는 연속으로 움직이므로, 점 하나만 찍으면 빠를 때 자국이 띄엄띄엄해진다. 직전 히트와 현재 히트 사이를 잇는 스트레치 브러시를 하나 더 찍고 이동 방향(`MoveYaw`)으로 회전시켜 궤적을 잇는다. 발자국 하나가 최대 두 브러시라, 버퍼가 `Num*2`였다.

## 4. GPU에 새기기: 인스턴스드 1드로우 + self-read

RT는 동적으로 읽고 쓸 수 있지만 **같은 패스에서 읽으면서 쓸 수는 없다.** 고전적 해법은 RT 세 장을 두고 핑퐁하는 것이다 — 현재 프레임 브러시용 RT_Current, 최종 높이를 담아 지면이 샘플하는 RT_Trails, 그리고 읽기/쓰기 충돌을 피하려 내용을 복사해 두는 RT_History. 나는 여기서 다른 길을 택했다. 다대다 전투에서 발자국이 폭증하니 브러시당 1드로우를 **인스턴스드 1드로우**로 합치고, 핑퐁 대신 **self-read** 한 번으로 끝냈다.

기성 발자국 구현은 퀄과 성능 이슈가 있어 따로 만들어야 했다. 버텍스 버퍼 없이 정점을 셰이더에서 만들고 인스턴스 버퍼만 인덱싱해, 발자국 N개를 드로우콜 한 번에 그린다.

``` hlsl
void MainVS(uint VertexID : SV_VertexID, uint InstanceID : SV_InstanceID,
            out float2 OutUV : TEXCOORD0, out nointerpolation uint OutInstance : TEXCOORD1,
            out float4 OutPosition : SV_Position)
{
    float2 corners[6] = { float2(0,0),float2(1,0),float2(1,1), float2(0,0),float2(1,1),float2(0,1) };
    float2 local = corners[VertexID];
    OutUV = local; OutInstance = InstanceID;

    FBrushInstance b = Instances[InstanceID];
    float2 centered = (local - 0.5f) * b.SizePx;             // 픽셀 공간에서 회전·평행이동
    float s = sin(b.Angle), c = cos(b.Angle);
    float2 pixel = b.CenterPx + float2(centered.x*c - centered.y*s, centered.x*s + centered.y*c);
    OutPosition = float4(pixel.x/MaskRes.x*2 - 1, 1 - pixel.y/MaskRes.y*2, 0, 1); // → 클립
}
```

self-read는 **같은 `FRHITexture`를 RTV로 쓰면서 raw SRV로 읽는** 방식이다. 마스크를 RTV로는 RDG에 등록하고(ELoad), 샘플용으로는 RDG가 추적하지 않는 raw `SHADER_PARAMETER_TEXTURE`로 따로 물린다. 같은 텍스처를 RTV+SRV로 동시에 바인딩하면 보통 RDG가 hazard로 막는데, raw SRV는 RDG 밖이라 이 검사를 우회하면서 Canvas처럼 코히런트하게 읽힌다.

``` cpp
FRDGTextureRef MaskRDG = RegisterExternalTexture(GraphBuilder, MaskTex, TEXT("DLWEMask"));

PassParams->MaskTexture      = MaskTex;   // 같은 FRHITexture를 raw SRV로 읽기 (self-read)
PassParams->RenderTargets[0] = FRenderTargetBinding(MaskRDG, ERenderTargetLoadAction::ELoad); // 쓰기

// over-blend: R_out = Vnew·df + R_dst·(1−df) = lerp(R_dst, Vnew, df)
PSOInit.BlendState = TStaticBlendState<
    CW_RED, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha,
            BO_Add, BF_Zero,        BF_One>::GetRHI();

RHICmdList.DrawPrimitive(0, 2, NumInstances);   // 2삼각형 × N인스턴스 = 1 드로우콜
```

핑퐁 없이 self-read가 성립하는 건 §1의 **min** 덕분이다. carve 바닥이 현재값과 무관하니, 인스턴스가 이웃이 쓰기 전 마스크를 읽든 후를 읽든 결과가 같다 — 순서 걱정 없이 읽어도 되고(stale read 무해), min·max 둘 다 순서무관이라 **인스턴스드(병렬 1드로우) == 브러시당 직렬 == 원래 Canvas 방식**이다. 실제로 평활화 항을 끄고 병렬과 직렬을 나란히 놓으면 눈으로 구별되지 않는다. 3 RT 핑퐁이 하던 "읽기/쓰기 분리"를, 순서무관 합성 + self-read가 한 패스로 대신한다.

브러시 인스턴스는 CPU에서 이렇게 패킹한다. 월드 좌표는 §5의 매핑으로 픽셀 좌표가 되고, 깊이·노이즈·둔덕 세기 등이 그대로 실린다.

``` cpp
FBrushInstanceGPU MakeBrushInstance(const FVector& Location, const FVector2D& SizeWorld, float AngleDeg,
                                    float DepthFraction, float TrueDepth, double Ratio, ...) const
{
    const float DeepAlpha = Clamp((TrueDepth - 14.f) / 26.f, 0.f, 1.f);
    const FVector2D BrushWorld = SizeWorld * Lerp(1.1f, 1.4f, DeepAlpha);   // 깊을수록 큰 자국

    FBrushInstanceGPU Inst;
    Inst.CenterPx  = FVector2f((Location.X - TopCorner.X) / Ratio, (Location.Y - TopCorner.Y) / Ratio);
    Inst.SizePx    = FVector2f(BrushWorld.Y / Ratio, BrushWorld.X / Ratio);
    Inst.Angle     = DegreesToRadians(AngleDeg);
    Inst.CompCurve = Lerp(CompCurveShallow, 1.f, Clamp(DepthFraction, 0.f, 1.f));
    Inst.CompNoise = Lerp(0.45f, 0.2f, DeepAlpha) * InteractionNoise;
    Inst.Piling    = EdgePiling * PilingRand;
    Inst.SmoothHeight = Lerp(0.6f, 1.f, DeepAlpha);
    return Inst;
}
```

## 5. 좌표 계약과 리센터

모든 RT는 플레이어를 따라다니는 유한한 창(window)이다. 지면 머티리얼(GPU)과 그리는 쪽(CPU)이 똑같이 변환하도록 매핑을 MPC 벡터 하나로 공유한다.

``` text
Mapping = float4( TopCorner.X, TopCorner.Y, RTWorldSize, CenterZ )
Ratio   = RTWorldSize / Resolution           // 픽셀 하나가 담는 월드 거리
pixel   = (worldXY - TopCorner) / Ratio       // CPU가 브러시를 찍을 때
uv      = (worldXY - TopCorner) / RTWorldSize // 머티리얼이 지면에서 읽을 때
```

플레이어가 움직이면 창도 따라가야 하는데, 그냥 옮기면 이미 그린 발자국이 화면에 붙어 끌려온다. 발자국은 월드에 고정돼야 하므로 창을 옮기는 만큼 RT 내용을 반대로 스크롤해 상쇄한다. 이때 **정수 텍셀 단위로만** 밀어야 한다 — 서브픽셀 오프셋은 매 프레임 bilinear 리샘플을 일으켜 궤적이 수묵화처럼 번진다(누적 blur). 두 프레임 위치를 그냥 빼면 UE의 1cm 정밀도로 나오지만 RT 픽셀은 보통 그보다 크므로(예: 창 8000 / 해상도), 반드시 픽셀 격자에 반올림해야 한다.

``` cpp
const double Ratio = SizeToResRatio();
const FVector2D PixelOffset(-RoundToDouble(Delta.X / Ratio),   // 정수 픽셀로 반올림 (텍셀 스냅)
                            -RoundToDouble(Delta.Y / Ratio));
ResolvedTop = TopCorner - PixelOffset * Ratio;                // TopCorner도 그 정수배로 스냅

// 스크래치 핑퐁으로 내용 스크롤: clear(rest) → 원본을 PixelOffset만큼 밀어 복사 → 되복사
UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, Scratch, Canvas, Size, Ctx);
Canvas->K2_DrawTexture(RT, PixelOffset, Full, FVector2D::ZeroVector, FVector2D::UnitVector,
                       FLinearColor::White, EBlendMode::BLEND_Opaque);
UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Ctx);
```

(이 핑퐁은 리센터 전용이다. §4의 브러시 그리기는 self-read라 핑퐁이 없다.)

## 6. 눈 더하기 / 빼기 — 영역 마스크

지면이 얼마나 하얀지(커버리지)는 별도 RT다. 특정 영역에 눈을 **더하는** 마스크는 폴오프 링을 버텍스 컬러(안쪽 1 → 가장자리 0)에 실은 프로시저럴 메시를 만들어, 위에서 내려다보는 오쏘 SceneCapture로 굽는다. 불규칙한 물체(쓰러진 캐릭터·말·소품)도 같은 방식으로 아래에서 위를 향하는 직교 캡처로 높이를 기록한다.

``` cpp
SceneCapture->ProjectionType     = ECameraProjectionMode::Orthographic;
SceneCapture->OrthoWidth         = WeatherRTWorldSize;
SceneCapture->PrimitiveRenderMode= PRM_UseShowOnlyList;    // 등록된 프록시 메시만
SceneCapture->bCaptureEveryFrame = false;                  // 상태 변할 때만
SceneCapture->SetWorldRotation(FRotator(-90, -90, 0));     // 수직 하방, 축을 월드 UV에 맞춤
SceneCapture->ShowFlags.DisableFeaturesForUnlit();         // 라이팅/그림자/포스트 제거
SceneCapture->ShowFlags.DisableAdvancedFeatures();         // SSR/Lumen/AO/TAA 제거

MeshComp->CreateMeshSection_LinearColor(0, Positions, Indices, Normals, UV0, Colors, {}, false);
SceneCapture->ShowOnlyComponents.AddUnique(MeshComp);
```

라이팅·그림자·포스트를 전부 끈 순수 언릿 캡처라, RT에는 버텍스 컬러(=커버리지 가중치)만 색으로 남는다. 반대로 눈을 **빼는** 마스크는 `RG8`에 브러시 텍스처(원형·피라미드·사각·팔로프)를 가산 스탬프해 커버리지에 곱한다. R=젖음 제거, G=눈/먼지 제거.

``` cpp
Canvas->K2_DrawTexture(ResolveBrushTexture(Comp), ScreenPos, PixelSize, UV0, UVSize,
    FLinearColor(Comp->MaskWetness, Comp->MaskSnowDust, 0, 1), BLEND_Additive, Yaw, FVector2D(0.5,0.5));
```

둘 다 §5의 텍셀-스냅 매핑을 공유한다.

## 7. 다시 쌓임

밟혀 압축된 눈은 눈이 더 내리면 메워져야 한다. refill 머티리얼을 마스크에 주기적으로 그려 값을 rest 쪽으로 되돌리는데, **그 주기를 현재 강설량으로 스케일**한다.

``` cpp
const float WeatherMax = FMath::Max(Weather->Snow, Weather->Dust);
const float X          = saturate(pow(WeatherMax / 10.f, 3.f));        // 강설 강도 (3제곱 램프)
const float Interval   = FadeStepBase / Lerp(FadeRateIdle, FadeRateActive, X); // 셀수록 짧은 간격

if ((FadeAccum += DeltaTime) < Interval) return;
FadeAccum = 0.f;
UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, RT, FadeMID);   // 한 스텝 메움
if (++CurrentFadeSteps >= MaxFadeSteps                                     // 충분히 메웠고
    && Weather->MaterialSnowCoverage <= 0 && Weather->MaterialDustCoverage <= 0) // 눈도 그쳤으면
    UKismetRenderingLibrary::ClearRenderTarget2D(World, RT, DlweClearColor);     // rest로 하드 clear
```

3제곱으로 밟은 게 포인트다. 흩날리는 눈엔 발자국이 거의 안 메워지고, 함박눈엔 빠르게 메워진다. 충분히 메워졌는데 눈까지 그치면 그때 한 번 rest로 하드 clear해 부동소수 찌꺼기를 정리한다. 같은 히스토리 채널로 트랙을 시간에 따라 서서히 회복시키거나(Preservation), 밟은 자리에 눈이 푹신하게 다시 차오르는(fluffy fall) 표현도 얹을 수 있다.

머티리얼 최적화를 하나도 안 하고 붙였는데 이 정도면 나쁘지 않다.

---

<font size="2">
참고: Naughty Dog, "The Technical Art of The Last of Us Part II" (SIGGRAPH 2020) · Square Enix, "Rendering Techniques in Rise of the Tomb Raider" (SIGGRAPH 2015) · Santa Monica Studio, "Reinventing the Wheel for Snow Rendering" (GDC 2023) · WB Games Montreal, "Deformable Snow Rendering in Batman: Arkham Origins" (GDC 2014).
</font>