---
layout: post
title: "Screen Space Decal"
categories: graphics
---

## overview

<!-- begin_excerpt -->

1. 씬의 깊이 정보에는 데칼 볼륨 깊이 정보도 기록 되있다.
1. 씬의 깊이 정보와 Zeye의 정보가 같다면 Xeye는 그대로 인정한다.
1. 박스 포함 씬 깊이라서 애초에 가정했던게 맞음
2. 근데 박스안의 깊이가 더 작은 이유는? 모르겠다



1. 박스를 그린다.
2. 픽셀 쉐이더에서 씬의 깊이 정보를 읽는다.
3. 클립 좌표를 deproject하여  Xeye 값을 읽는다.
4. 씬의 깊이 값과 클립의 깊이 값의 차이가 없다면 Xeye는 원본 값으로 유지된다.
5. 씬 깊이가 더 크다면
  10 / 2 = 5 
6. 씬 깊이가 더 작다면 
  10 / 20 = 0.5 컬링 된다.


<!-- end_excerpt -->

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal.png" width="100%">
  <figcaption></figcaption>
</div>
</figure>

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/decal3.png" width="100%">
  <figcaption></figcaption>
</div>
</figure>
