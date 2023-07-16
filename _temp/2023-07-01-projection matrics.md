---
layout: post
title: "projection matrics"
categories: graphics
---

3차원의 한점은 우리가 보는 투영공간으로 변환되어 원근감을

살리고 정규화 과정을 거쳐 2D의 한점으로 변환 된다. 

2D 화면상의 한점을 3D상의 한점으로 역변환 할 수 있을까?

## convert to projection space

우리가 보는 3차원상의 점 Xₑ는 투영 평명(near plane)상 점 Xₚ로 

매핑된다.

Xₚ는 한 변의 각도가 같은 비슷한 직각 삼각형이 같은 비율을 가지는 성질을 

이용해 다음과 같이 쓸 수 있다.

Xₚ / Zₚ = Xₑ / Zₑ 이고 구하고자 하는 Xₚ 로 정리하면

Xₚ = Zₚ / Zₑ * Xₑ 이다.


## normalized device coordinates

원근 적용된 Xₚ는 

2D 화면 좌표계 변환전 perspective divide를 

통한 NDC 좌표계로의 변환 과정을 거친다.

이는 데카르트 좌표계로 표현하지 못하는 원근감을 깊이 값 z를 차수로 한 동차 

좌표계로 변환후 역변환하는 과정이다.











[​l, r]을 [0, 1]로 normalize

x' = (x - min) / (max - min)

[0, 1]을 [-1, 1]로 linear transform

x' = (max - min)/(1 - 0) * x + min

x' = 2 * x




[0, 1]을 [-1, 1]로 맵핑 할때 

아래의 min-max scaling 공식을 사용하면

x' = (x - min) / (max - min)

아래처럼 나올텐데

y = (? - min) / (max - min)

y = (-1 + 1) / 2


x에 0을 넣으면 -1이 안나옴




xₙ = (Xₚ - r) / (l - r)

[0, 1]을 [-1, 1]로 스케일 맵핑



y = (1 - (-1)) * x + (-1)

xₙ = (1 - (-1)) * (Xₚ - r) / (l - r) + (-1)

xₙ = 2 * (Xₚ - r) / (l - r) - 1

xₙ = (2ZₚXₑ - 2rZₑ) / (Zₑl - Zₑr) - 1




y = ax + c

[l ~ r]을 [-1 ~ 1]로 맵핑


우선 정규화를 


[l ~ r] -> [0 ~ 1] -> [-1 ~ 1]


어떤 범위를 선형적으로 다른 범위로 변환하는 걸 선형변환이라 한다

(Xₚ - l)/(r - l)



