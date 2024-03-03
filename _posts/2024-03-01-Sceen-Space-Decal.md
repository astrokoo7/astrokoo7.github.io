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

어떤 공간상의 특정 위치에 점이 있다고 가정해보자.<br>
그 점의 위치는 그 공간상의 좌표계로 표현할 수 있다. <br>
가령 x가 1 y는 2 z는 3처럼 말이다. 

이때 그 점을 어떤 필요에 의해 다른 공간으로 선형 변환했다고 생각해보자.<br>
그럼 그 점은 새로운 공간의 좌표계로 표현되어 x는 3 y는 2 z는 -1처럼 다르게 표현 될 수 있다.

하지만 점을 표현하는 값이 좌표계에 따라 달라졌을뿐 실제 공간상의 점의 위치 자체는 변하지 않는다.

화면 좌표계상 문양이 그려질 Decal 박스의 각 pixel의 좌표와 그 좌표에 해당하는 깊이 값을 NDC 좌표계로 변환할 수 있다. 

NDC 좌표계로 변환 된 Decal 박스 각 pixel의 점들은 Mesh 표면의 공간상 좌표를 포함하고 있다.

Mesh 표면의 공간상 좌표는 정의되었지만 Decal 박스 중 Mesh 표면이 아닌 좌표는 버려 Mesh 표면에 해당하는 Decal 박스 영역만 그려지는 처리가 필요하다.

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


... in progress









             