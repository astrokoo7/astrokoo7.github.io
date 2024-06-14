---
layout: post
title: "gamma space & sRGB"
categories: graphics
---

<!-- begin_excerpt -->

사람은 감각에 따라 자극의 정도가 특정 지수 값에 비례한다고 알려져 있다.

<!-- end_excerpt -->

그중 색상 차의 미묘한 변화는 알아차리기 어렵다고 한다.

만약 0은 검은색 1은 흰색일 때,

검은색에서 흰색으로의 변화를 일정하게 표현 한다면 색상 차의 미묘한 변화를 

알아차리지 못해 아래 이미지처럼 검은색에서 흰색으로의 급격한 변화를 느낀다.

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/gamma1.png" width="100%">
    <figcaption></figcaption>
</div>
</figure>

gamma space란 이러한 감각적 특정에 맞춰 색상 차를 잘 인지할 수 있도록 보정해주는 기법으로

아래처럼 사람이 변화를 구분하기 쉽게 도와준다.

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/gamma2.png" width="100%">
    <figcaption></figcaption>
</div>
</figure>

