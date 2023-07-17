---
layout: post
title: "projection matrics"
categories: graphics
---

시야 기준 3D 가상 공간을 표현하는 데카르트 좌표계상 한점 P_{eye}는 

깊이 값 z를 동차좌표계의 차수 w로 지정하여 만들고 데카르트 좌표계로 역변환 

(perspective divid) 과정을 통해 데카르트 좌표계상 원근감을 생성한다.

이때 동차좌표계 변환시 (x_{eye}, y_{eye}, z_{eye})에 깊이 값 z만 w로 지정하여 

변환 후 데카르트 좌표계로 역변환하면 아래와 같고

$$

(\frac x_{eye} w_{eye}, \frac y_{eye} w_{eye}, \frac z_{eye} w_{eye} , \frac w_{eye} w_{eye}) 

$$

깊이 값 z_{eye}와 w_{eye}은 서로 같아 역변환시 차수 w에 따른 깊이감이 없어지고 

시야각도에 따른 원근감을 표현 할 수 없는 문제가 있다.

이에따라 동차좌표계 변환시 시야각도에 따른 깊이값에 따른 좌우/상하 비율값을

적용하여 x_{clip}, y_{clip}을 계산하고 다른 값에 독립적인 깊이 값 z_{clip}은

선형 변환으로 계산하여 깊이 식별과 시야각에 따른 원근감을 표현 할 수 있다.









$$

P_{clip} = M_{projection} \times P_{eye}

P_{clip} = (X_{clip}, Y_{clip}, Z_{clip}, W_{clip})

P_{ndc}= \frac {P_{clip}} {W_{clip}}

$$



$$

\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
M_{projection} 
\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}

$$




좌우/상하 값인 x, y는 동차좌표계 변환시 



시야 속성 fov 값에 따른 깊이 

z를 사용한 비율로 표현되며 깊이 z는 단독으로 선형 변환을 통해 표현된다.








깊이 값이 z는 동차좌표계로 변환시 시야 속성














## convert to projection space

우리가 보는 3차원상의 점 Xₑ는 투영 평명(near plane)상 점 Xₚ로 

매핑된다.

Xₚ는 한 변의 각도가 같은 비슷한 직각 삼각형이 같은 비율을 가지는 성질을 

이용해 다음과 같이 쓸 수 있다.

Xₚ / Zₚ = Xₑ / Zₑ 이고 구하고자 하는 Xₚ 로 정리하면

Xₚ = Zₚ / Zₑ * Xₑ 이다.

Xₓ
## normalized device coordinates

원근 적용된 Xₚ는 

2D 화면 좌표계 변환전 perspective divide를 

통한 NDC 좌표계로의 변환 과정을 거친다.

이는 데카르트 좌표계로 표현하지 못하는 원근감을 깊이 값 z를 차수로 한 동차 

좌표계로 변환후 역변환하는 과정이다.






Xₓ




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



