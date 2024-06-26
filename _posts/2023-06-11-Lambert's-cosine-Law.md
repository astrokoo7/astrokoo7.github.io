---
layout: post
title: "Lambert's cosine law"
categories: graphics
---

## background

<!-- begin_excerpt -->

램버시안 코사인 법칙은 어떤 표면을 관찰자가 볼때 바라보는 위치에 상관 없이 빛이 표면에 입사하는 각도에 따라 반사하는 광량을 설명한다.

<!-- end_excerpt -->

<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/diffuse.png">
</div>

빛은 반사되는 재질에 따라 거울처럼 입사한 빛이 흐트러지지 않고 그대로 반사되기도 하며 돌이나 나무 같은 불투명 재질에선 입사한 빛이 재질 내부에서 상호작용하여 여러 방향으로 갈라져 난반사 된다.

빛이 불투명 재질에서 난반사 될때 고른 분포로 난반사 되는 재질을 램버시안 표면이라 부른다.

현실에선 순수한 램버시안 표면은 없지만 흰색 종이나 원목 같은 고른 표면의 재질은 일정한 분포로 난반사 된다.


## cosine law

램버시안 표면에서 입사하는 빛은 왜 굳이 코사인 비율로 광량이 줄어들까?
각도에 따라 선형으로 광량이 줄어들지 않는 이유는 무얼까?

<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/lambertian_intensity.png" width="82%">
</div>

한줄기 빛이 수직으로 표면에 비출때 표면 중심으로부터 원형으로 난반사 된다면 자연스러울 것이다.

원의 표면 중심점을 기준으로 반대쪽 원의 한점을 각도에따라 직선을 그어보면 그 직선의 길이는 각도에 따른 cos 값임을 알 수 있어 왜 빛이 원형으로 난반사 될때 cos 수치를 따르는지 설명이 된다.

<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/cosin.png" width="82%">
</div>

랜더링할 때 버텍스 노멀과 빛의 방향 하나로 난반사를 모사한다.

표면이 평평하지 않다면 빛의 방향에 따른 노멀과의 각도에 따라 마찬가지로 cos 비율로 광량이 표현 될 것이다.
























