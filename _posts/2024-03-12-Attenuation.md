---
layout: post
title: "Attenuation"
categories: graphics
---

## Overview

<!-- begin_excerpt -->
빛은 이동한 거리에 따라 그 세기가 감소한다. 수성과 금성이 지구와 화성보다 뜨거운 이유다.
<!-- end_excerpt -->
<br>
태양으로부터 발생한 빛은 공간을 지나면서 흡수되거나 산란하여 거리가 멀어질수록 그 세기가 감소한다. <br>
이를 기술적으로 빛의 감쇠(light attenuation)라고 부른다. <br>
<br>
빛이 통과하는 공간에 따라 감쇠하는 양이 다르지만 보통 공기중에선 [이동한 거리의 제곱에 반비례](https://en.wikipedia.org/wiki/Attenuation)한다고 알려져 있다. <br>

이는 식 \\(attenuation = \frac 1 {d^2}\\)로 쓸 수 있는데 거리 d 값이 증가함에 따라 급격한 감쇠를 보인다.<br>

또한 위 식은 1을 기점으로 거리 d가 1보다 작다면 빛은 감쇠하지 않고 크다면 감쇠하여 0.0에서 1.0의 범위를 가진다.<br>

> <font size="2"> 
> 감쇠 값은 1보다 큰 값은 1로 맞추고 거리가 멀어질수록 0에 무한히 가까워지지만 0은 되지 않는다.
> </font>



빛의 감쇠는 단순히 어떤 물체의 원본 색상을 얼마의 비율로 픽셀에 적용할지 계산하는데 사용한다.<br>






