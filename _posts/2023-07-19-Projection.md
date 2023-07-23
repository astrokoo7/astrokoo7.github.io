---
layout: post
title: "Perspective Projection (draft)"
categories: graphics
---

## overview

시야 기준 3D 가상 공간을 표현하는 데카르트 좌표계상 한점 \\( (x_{eye}, y_{eye}, z_{eye}) \\)는 

깊이 값 \\( z_{eye} \\)를 차수 \\( w_{clip} \\)로 지정하여 동차좌표계(clip space)상 

한점 \\( (x_{clip}, y_{clip}, z_{clip}, w_{clip}) \\)로 변환 후 다시 데카르트 좌표계로

역변환 (perspective divide) 과정을 거쳐 데카르트 좌표계상 원근감을 생성한다.

역변환 (perspective divide) 식은 다음과 같고 \\(
(\frac {x_{clip}} {w_{clip}}, \frac {y_{clip}} {w_{clip}}, \frac {z_{clip}} {w_{clip}} , \frac {w_{clip}} {w_{clip}}) 
  \\)

이를 그림으로 표현하면 아래와 같은 식이다.

![alt]({{ site.url }}{{ site.baseurl }}/assets/images/homogeneous.png)

$$

$$



이런 단순 변환은 깊이 값 \\( Z_{clip} \\)와 \\( W_{clip} \\)가 서로 같아 데카르트 좌표계로 

역변환시 차수 \\( W_{clip} \\)에 따른 깊이감이 없어지고 시야각도별 차등 원근감을 

표현 할 수 없는 문제가 있다.

이러한 문제를 해결하기 위해 동차좌표계(clip space)로 변환시 좌우/상하 

값은 시야각에 따라 원근감의 정도가 달라져야하고 깊이 값은 데카르트 좌표계로

역변환시 Near(0)와 Far(W) 기준으로 거리 값이 식별되도록 고려해야 한다.

