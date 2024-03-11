---
layout: post
title: "Screen Space Decal"
categories: graphics
---

## overview

Decal은 Mesh의 표면을 따라 특정 문양을 그려주는 기술로 Screen Space Decal이 소개되기 이전엔 투사될 Mesh에 문양이 그려질 부분에 해당하는 Decal Mesh를 똑같이 만들어 Mesh위에 살짝 띄워 그려주는 방식을 사용했다. 

<!-- begin_excerpt -->
이 방식은 직관적이지만 까다로운 구현이 필요한데 간단히 요약하면 타겟 Mesh에 문양이 그려질 공간을 직교 투영(Orthogonal) 박스로 투영하여 공간안에 있는 Mesh의 vertex를 추려내고 서로 다시 이어붙여 문양이 그려질 표면 Mesh를 생성하는 식이다.
<!-- end_excerpt -->
> <font size="2"> 
> 이는 GPU에서 clip space밖의 vertex를 clipping하는 방법과 동일하다.
> </font>

또한 표면 Mesh의 vertex를 투영 공간으로 변환하여 계산하는데 이때의 투영 공간상 vertex 좌표는 그대로 표면 Mesh의 uv texture 좌표계로 변환하여 사용하면 된다.

[여기에 더 자세한 설명이 있으니 참고 바란다.](http://blog.wolfire.com/2009/06/how-to-project-decals/)


하지만 이러한 방법은 동적 Decal로 사용하기엔 매번 새로운 메쉬를 생성해야하고 타겟 메쉬의 버텍스가 많다면 계산량이 많아져 성능에 영향을 많이 미친다.

## Screen Space Decal

어떤 공간상의 특정 위치에 점이 있다고 가정해 보자.<br>
그 점의 위치는 그 공간상의 좌표계로 표현할 수 있다. <br>
가령 x가 1 y는 2 z는 3처럼 말이다. 

이때 그 점을 어떤 필요에 의해 다른 공간으로 선형 변환했다고 생각해 보자.<br>
그럼 그 점은 새로운 공간의 좌표계로 표현되어 x는 3 y는 2 z는 -1처럼 다르게 표현될 수 있다.

하지만 점을 표현하는 값이 좌표계에 따라 달라졌을 뿐 실제 공간상의 점의 위치 자체는 변하지 않는다.

문양을 그려 넣을 Mesh가 2차원 화면에 그려진 상태에서 화면상 그려 넣을 2차원 표면을 3차원 좌표계로 변환할 수 있을까?

화면 좌표계상 문양이 그려질 Decal 박스가 있다고 생각하고 박스의 2차원상 pixel 좌표와 그 좌표에 해당하는 깊이 값을 NDC 공간 좌표계로 변환해보자.

NDC 좌표계로 변환된 Decal 박스의 각 pixel들은 Mesh 표면의 공간상 좌표를 포함하고 있다.

Mesh 표면의 공간상 좌표는 정의되었지만, Decal 박스 중 Mesh 표면이 아닌 좌표는 버려 Mesh 표면에 해당하는 Decal 박스 영역만 그려지는 처리가 필요하다.

Decal 박스 pixel의 공간상 점을 Decal 박스 로컬 좌표계로 역변환하여 박스 영역을 벗어나는 점은 버리고 Mesh 표면에 해당하는 Decal 박스 영역만 그릴 수 있다.

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303.png" width="70%">
  <figcaption>Decal 박스의 마름모 점은 scene의 깊이맵만큼 투영되어 X로 이동된다.</figcaption>
</div>
</figure>


<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-2.png" width="40%">
  <figcaption>Scene의 깊이로 투영된 Decal 박스의 화면 좌표가 로컬 좌표계로 변환 후 <br>Decal 박스 영역을 벗어나면 그리지 않는다. </figcaption>
</div>
</figure>


<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-3.png" width="30%">
  <figcaption>그릴 영역만 남은 Decal 박스의 pixel은 Decal 박스 로컬 좌표계 x,z축 값을<br> 그대로 uv 좌표계로 변환(+0.5)하여 문양을 그리면 된다. </figcaption>
</div>
</figure>

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal0303-4.png" width="50%">
  <figcaption> 데칼을 지형 표면에 그렸다. </figcaption>
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
                float4 positionCS = float4(screenUv * 2.0 - 1.0, depth, 1.0);
                positionCS.y = -positionCS.y;
                float4 hpositionWS = mul(UNITY_MATRIX_I_VP, positionCS);
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

위 코드는 간단한 SSD 코드로 유니티 shader 코드를 사용해 작성되었다.

좌표 변환 관련을 코드를 살펴보면 우선 `_ScaledScreenParams.xy` 값은 unity의 내장 변수로 view port 정보를 가지고 있다.

이 값을 사용해 입력으로 들어온 화면 좌표계상 xy 좌표를 스크린 UV 좌표로 변환하여 해당 좌표의 Scene 깊이 값을 가져온 뒤 화면 좌표계의 xy와 scene의 깊이로 clip space로의 좌표 변환을 해주는 부분을 볼 수 있다.

그다음 clip space 좌표를 역행렬 UNITY_MATRIX_I_VP를 곱하여 월드 좌표계로 변환 후 역수 값인 w를 역으로 나눠줌으로서 실제 월드 좌표계 값으로 변환을 했다.

이후 월드 좌표계상 pixel 좌표를 Decal 박스의 로컬 좌표계로 다시 변경하여 로컬 좌표계상 버텍스의 범위인 -0.5, 0.5를 넘지 않는지 확인하여 넘는다면 버림 처리를 하였다.

> <font size="2"> 
>애초에 Decal 박스는 -0.5, 0.5의 범위를 가지는 박스이고 실제 월드에 배치하여 사용할 땐 크기를 늘리거나 회전시키는 등 필요에 맞춰 사용한다.
> </font>

또한 화면상의 점이 박스 안의 pixel이라면 그대로 Decal 박스의 로컬 좌표계에 맞춰 uv 좌표로 지정하여 texture의 색상을 읽으면 된다.

마지막으로 눈에 보이는 texture 문양은 Decal 박스의 로컬 좌표계 xz 축만 사용하여 높이 값이 없다. 즉 로컬 좌표계로 변환된 값이 xz는 같지만 y인 높이 값만 다르다면 같은 texutre unit을 셈플링하여 길게 늘어지는 현상이 발생하게 된다. 이런 경우 데칼의 투영 방향과 Mesh 표면의 Normal 방향이 같다면 버림처리를 해준다.





             