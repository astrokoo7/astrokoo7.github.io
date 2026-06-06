---
layout: post
title: "Screen Space Decal"
categories: graphics
---

## background

데칼은 물체의 표면을 따라 런타임에 텍스처를 입혀주는 기술이다. Screen Space Decal이 소개되기 이전에는, 텍스처가 그려질 물체의 표면을 그대로 본뜬 데칼 메쉬를 만들어 텍스처를 입힌 뒤 물체 위에 살짝 띄워 그려주는 방식을 썼다.

<!-- begin_excerpt -->
이 방식은 직관적이지만 구현이 까다롭다. 텍스처를 입힐 물체를 직교 투영(Orthographic) 박스로 투영한 뒤, 투영 공간 안에 들어온 물체의 vertex만 추려 서로 다시 이어 붙여 데칼 메쉬를 만드는 식이다.
<!-- end_excerpt -->
> <font size="2"> 
> 이는 GPU에서 clip space 밖의 vertex를 clipping하는 방법과 동일하다.
> </font>

[여기에 더 자세한 설명이 있으니 참고 바란다.](http://blog.wolfire.com/2009/06/how-to-project-decals/)

하지만 이 방법은 동적 Decal로 쓰기엔 매번 새 메쉬를 생성해야 하고, 타겟 메쉬의 버텍스가 많으면 계산량이 늘어 성능에 큰 영향을 준다.

## Screen Space Decal

어떤 공간의 특정 위치에 점이 있다고 하자. 그 점은 해당 공간의 좌표계로 표현된다. 가령 (x, y, z) = (1, 2, 3)처럼 말이다. 이 점을 필요에 따라 다른 공간으로 선형 변환하면, 새 공간의 좌표계에서는 (3, 2, -1)처럼 다른 값으로 표현된다. 하지만 좌표계에 따라 값이 달라졌을 뿐, 실제 공간상의 점의 위치 자체는 변하지 않는다.

같은 맥락에서, 3차원 공간의 물체를 어떤 규칙으로 2차원 공간에 투영했다면, 그 규칙을 역으로 사용해 2차원 공간의 점을 다시 3차원 시점에서 바라볼 수 있다.

이를 이용해, 3차원 공간에서 데칼이 그려질 물체를 감싸는 박스를 배치하고 그 박스를 2차원 공간으로 투영하면, 화면상 박스의 표면과 박스 안 물체의 표면을 2차원 좌표계에서 정의할 수 있다. 다만 2차원 좌표계만으로는 박스와 물체의 표면이 겹치는 좌표를 따로 분리해 낼 수 없는데, 이 표면 좌표를 다시 3차원 공간으로 변환하면 구분할 수 있다.

물체가 포함된 박스의 표면 좌표를 3차원으로 변환할 때, x·y는 그대로 매핑하고 깊이 값만 화면 좌표계상 표면이 가리키는 씬의 깊이로 변환해 보자. 그러면 박스와 물체의 표면이 겹치는 곳의 씬 깊이 값은 3차원상 박스 안으로, 겹치지 않는 표면은 박스 밖으로 변환된다. 2차원에서 구분할 수 없었던 "물체와 겹치지 않는 박스 표면"을 3차원 변환으로 구분할 수 있게 된 것이고, 이를 통해 데칼을 물체 표면에만 입힐 수 있다.


<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303.png" width="60%">
  <figcaption>Decal 박스의 마름모 점은 scene의 깊이맵 만큼 투영되어 X로 이동 된다.</figcaption>
</div>
</figure>

또한 변환된 표면의 3차원 카메라 좌표계 좌표를 박스의 로컬 좌표계로 한 번 더 변환하면, AABB 계산 없이 로컬 좌표계상 디폴트 크기 1을 넘었는지로 손쉽게 구분할 수 있다. 더불어 로컬 좌표계의 x·z 값을 그대로 텍스처의 uv 좌표로 사용할 수 있다.

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-2.png" width="60%">
  <figcaption>Scene의 깊이로 투영된 Decal 박스의 화면 좌표가 로컬 좌표계로 변환 뒤 <br>Decal 박스 크기를 벗어나면 그리지 않는다. </figcaption>
</div>
</figure>


<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-3.png" width="60%">
  <figcaption>그릴 영역만 남은 Decal 박스의 pixel은 Decal 박스 로컬 좌표계 x,z축 값을<br> 그대로 uv 좌표계로 변환(+0.5)하여 텍스쳐를 입히면 된다. </figcaption>
</div>
</figure>

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-4.png" width="60%">
  <figcaption> 데칼을 지형 표면에 입혔다. </figcaption>
</div>
</figure>

``` hlsl
Shader "Decals/ScreenSpaceOriginal"
{
    Properties
    {
        [NoScaleOffset]
        _MainTex("Texture", 2D) = "white" {}
    }
    SubShader
    {
        Tags { "RenderType" = "Opaque" }

        Pass
        {
            HLSLPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma multi_compile_fog

            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/DeclareDepthTexture.hlsl"
            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/DeclareNormalsTexture.hlsl"

            struct appdata
            {
                float4 vertex : POSITION;
            };

            struct v2f
            {
                float4 vertex : SV_POSITION;
                float4 fogFactor : TEXCOORD2;
            };

            TEXTURE2D(_MainTex);
            SAMPLER(sampler_MainTex);

            v2f vert(appdata v)
            {
                v2f o;
                o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
                o.fogFactor = ComputeFogFactor(o.vertex.z);
                return o;
            }

            float3 ComputeWorldPosition(float2 screenUv)
            {
                float depth = SampleSceneDepth(screenUv);
                float4 positionNdc = float4(screenUv * 2.0 - 1.0, depth, 1.0);
                positionNdc.y = -positionNdc.y;
                float4 hpositionWS = mul(UNITY_MATRIX_I_VP, positionNdc);
                return hpositionWS.xyz / hpositionWS.w;
            }

            float4 frag(v2f i) : SV_Target
            {
                float2 screenUv = i.vertex.xy / _ScaledScreenParams.xy;
                float3 worldPosition = ComputeWorldPosition(screenUv);
                float3 localPosition = mul(UNITY_MATRIX_I_M, float4(worldPosition, 1.0)).xyz;
                float3 worldNormal = SampleSceneNormals(screenUv);
                float3 decalDirection = TransformObjectToWorldDir(float3(0,1,0));

                float d = dot(decalDirection, worldNormal);
                clip(d);
                clip(0.5f - abs(localPosition.xyz));

                float2 decalUv = localPosition.xz + 0.5;
                float4 color = SAMPLE_TEXTURE2D(_MainTex, sampler_MainTex, decalUv);

                float normalFading = smoothstep(0, 1, saturate(d));
                color.rgb = MixFog(color.rgb, i.fogFactor) * normalFading;
                return color;
            }
            ENDHLSL
        }
    }
}
```

위 코드는 유니티 shader로 작성한 간단한 SSD 예제다. 좌표 변환 부분을 살펴보면, 우선 `_ScaledScreenParams.xy`는 유니티 내장 변수로 viewport 정보를 담고 있다. 이 값으로 입력된 화면 좌표계의 xy를 스크린 UV로 변환하고, 해당 UV의 Scene 깊이 값을 가져온다. 그런 다음 화면 좌표계의 xy와 씬 깊이를 묶어 NDC 좌표를 구성한다.

이렇게 만든 NDC 좌표에 역행렬 `UNITY_MATRIX_I_VP`를 곱해 동차 월드 좌표를 얻고, 마지막에 w로 나눠(perspective divide) 실제 월드 좌표로 변환한다. 이후 월드 좌표계의 픽셀 좌표를 다시 Decal 박스의 로컬 좌표계로 변환하여, 로컬 좌표가 박스 범위인 -0.5 ~ 0.5를 넘으면 버림 처리한다.

> <font size="2"> 
> 애초에 Decal 박스는 -0.5 ~ 0.5 범위를 가지는 박스다. 실제 월드에 배치할 땐 크기를 늘리거나 회전시키는 등 필요에 맞춰 쓰면 된다.
> </font>

박스 안에 남은 로컬 좌표는 +0.5 해서 0 ~ 1 범위의 uv로 변환한 뒤 텍스처 색상을 읽는다. 끝으로, 텍스처는 Decal 박스 로컬 좌표계의 xz 축만 사용하므로 높이(y) 정보가 없다. 즉 xz는 같고 y만 다른 픽셀들이 같은 텍셀을 샘플링해 텍스처가 길게 늘어지는 현상이 생긴다. 이런 경우 데칼의 투영 방향과 물체 표면의 Normal 방향이 같을 때만 그리고, 그렇지 않으면 버림 처리한다.
