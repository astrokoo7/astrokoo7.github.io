---
layout: post
title: "Perspective Projection (draft)"
categories: graphics
---

## overview

시야 기준 3D 가상 공간을 표현하는 데카르트 좌표계상 한점 \\( (x_{eye}, y_{eye}, z_{eye}) \\)는

깊이 값 \\( z_{eye} \\)를 차수 \\( w_{clip} \\)로 지정하여 동차좌표계(Clip Space)상 한점 

\\( (x_{clip}, y_{clip}, z_{clip}, w_{clip}) \\)로 변환 후 다시 데카르트 좌표계로 역변환 

(Perspective Divide) 과정을 거쳐 데카르트 좌표계상 원근감을 생성한다.

Perspective Divide는 다음과 같고 \\(
(\frac {x_{clip}} {w_{clip}}, \frac {y_{clip}} {w_{clip}}, \frac {z_{clip}} {w_{clip}} , \frac {w_{clip}} {w_{clip}}) 
  \\)

이를 그림으로 표현하면 아래와 같은 식이다.

![alt]({{ site.url }}{{ site.baseurl }}/assets/images/homogeneous.png)

$$

$$



이런 단순 변환은 깊이 값 \\( z_{clip} \\)와 \\( w_{clip} \\)가 서로 같아 데카르트 

좌표계로 역변환시 차수 \\( w_{clip} \\)에 따른 깊이감이 없어지고 시야각도별 차등 

원근감을 표현 할 수 없는 문제가 있다.

이러한 문제를 해결하기 위해 동차좌표계(Clip Space)로 변환시 좌우/상하 

값은 시야각에 따라 원근감의 정도가 달라져야하고 깊이 값은 데카르트 좌표계로

역변환시 Near(0)와 Far(W) 기준으로 거리 값이 식별되도록 고려해야 한다.

## perspective projection matrix

![alt]({{ site.url }}{{ site.baseurl }}/assets/images/gl_projectionmatrix01.png)

원근 투영을 통해 view space을 clip space로 변환하고 perspective divide를 통해

clip space를 normalize device space로 변환한다.

clip space는 동차좌표계 공간으로 데카르트 좌표계로의 변환을 염두하여 동차좌표계로

의 변환식을 정리하는데 view space상 절두체 공간 안의 점\\((x_{eye}, y_{eye})\\)는

절두체 앞면(Near Plan)으로 각도에 따른 직각 삼각형 비율에 맞춰 변환식을 작성하고 

깊이 값 \\(z_{eye}\\)는 선형 방정식의 2개의 계수를 알려진 상수 값 near와 far거리로

유도하여 변환식을 작성한다.

이러한 과정을 통해 3차원 시야 공간의 점들은 깊이 값을 가진 2차원상의 점으로 변환되어 

2D 공간에 그려질 준비가 된다.











