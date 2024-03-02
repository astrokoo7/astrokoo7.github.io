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

Screen Space Decal은 문양을 위한 공간을 그릴 때 스크린상에서 처리하는 기술로 추가적인 Mesh 생성이 필요하지 않고 깊이 텍스트를 활용하여 그린다.