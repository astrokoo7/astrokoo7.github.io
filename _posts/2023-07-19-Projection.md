---
layout: post
title: "Perspective Projection (draft)"
categories: graphics
---

## overview

<!-- begin_excerpt -->

시야 기준 3D 가상 공간을 표현하는 데카르트 좌표계상 한점 \\( (x_{eye}, y_{eye}, z_{eye}) \\)는 깊이 값 \\( z_{eye} \\)를 차수 \\( w_{clip} \\)로 지정하여 동차좌표계(Clip Space)상 한점 \\( (x_{clip}, y_{clip}, z_{clip}, w_{clip}) \\)로 변환 후 다시 데카르트 좌표계로 역변환(Perspective Divide)하여 데카르트 좌표계상 원근감을 생성한다.

<!-- end_excerpt -->

Perspective Divide는 다음과 같고 \\((\frac {x_{clip}} {w_{clip}}, \frac {y_{clip}} {w_{clip}}, \frac {z_{clip}} {w_{clip}} , \frac {w_{clip}} {w_{clip}})\\)

이를 그림으로 표현하면 아래와 같은 식이다.

<div style="text-align: center">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/homogeneous.png" width="58%" >
</div>

$$

$$



하지만 이런 단순 변환은 깊이 값 \\( z_{clip} \\)와 \\( w_{clip} \\)가 서로 같아 데카르트 좌표계로 역변환시 차수 \\( w_{clip} \\)에 따른 깊이감이 없어지고 시야각도별 차등 원근감을 표현 할 수 없는 문제가 있다.

이러한 문제를 해결하기 위해 동차좌표계(Clip Space)로 변환시 좌우/상하 값은 시야각에 따라 원근감의 정도가 달라져야하고 깊이 값은 데카르트 좌표계로 역변환시 near와 far 기준으로 거리 값이 식별되도록 고려해야 한다.

## perspective projection matrix

<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/gl_projectionmatrix01.png" width="65%">
</div>

perspective projection matrix는 시야 공간을 원근 투영(동차좌표계)과 NDC(데카르트좌표계) 변환 두개를 고려한 한개의 행렬식이다.

원근 투영을 통해 view space을 clip space로 변환하고 perspective divide를 통해 clip space를 normalize device space로 변환한다.

clip space는 동차좌표계 공간으로 데카르트 좌표계로의 변환을 염두하여 동차좌표계로의 변환식을 정리하는데 view space상 절두체 공간 안의 점 \\((x_{eye}, y_{eye})\\)는 절두체 앞면(near plan)으로 각도에 따른 직각 삼각형 비율에 맞춰 변환식을 작성하고 깊이 값 \\(z_{eye}\\)는 선형 방정식을 통해 변환식을 작성하는데 2개의 미지의 계수를 알려진 상수 값 near와 far로 유도하여 작성한다.

이러한 과정을 통해 3차원 시야 공간의 점들은 깊이 값을 가진 2차원상의 점으로 변환되어 2D 공간에 그려질 준비가 된다.


## each space in shader

vertex shader `vert`의 입력 `v.vertex`는 object/module space로 모델의 vertex 값\\((x_{model}, y_{model}, z_{model}, 1)\\)을 입력으로 받게된다.

``` 
v2f vert (appdata v)
{
  v2f o;
  o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);
  return o;
}
```

vertex shader의 출력 `o`의 `o.vertex`는 clip space로 변환 된 점 \\((x_{clip}, y_{clip}, z_{clip}, w_{clip})\\)로 각각 `[-w, w], [-w, w], [0, far], [near, far]`의 범위를 가진다.

vertex shader 이후 GPU는 자동으로 perspective divide를 실행하여 vertex shader의 `o.vertex`를 ndc space로 변환 해주어 각각 `[-1, 1], [-1, 1], [0, 1], [near, far]`의 범위를 가지게 된다.

ndc space로 변환된 vertex는 GPU에 의해 한번더 window space(실제 창 크기를 가지는 좌표계)로 변환 되며 각각 `[0, screenPixelWidth], [0, sceenPixelHeight], [0, 1], [near, far]`의 범위값을 가지게 된다.

마지막으로 window space로 변환된 vertex는 fragment shader `frag`의 입력 `i.vertex`로 전달된다.

```
fixed4 frag (v2f i) : SV_Target
{
  return fixed4(1, 1, 1, 1);
}
```

여기서 주목할 점은 clip space에서 clipping이 일어나 카메라 영역밖의 vertex는 비용이 비싼 perspective divide를 피하여 성능 최적화를 했다는 점도 참고하자.






















