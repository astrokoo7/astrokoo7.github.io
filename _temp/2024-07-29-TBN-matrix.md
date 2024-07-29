---
layout: post
title: "TBN matrics"
categories: c++
---

## Tangent Space

..ing

## TBN matrics

TBN 행렬은 텍스쳐 공간(tangent space)에서 모델 공간으로의 변환 

및 역변환을 다룬다는 점에서 서로 다른 공간 간 변환을 다루는 MVP 

행렬과 같은 목적성을 가진다.

<!-- begin_excerpt -->

하지만 TBN 행렬을 사용한 공간 변환은,

MVP 행렬을 사용한 변환처럼 기하학적으로 직관적이지 않다.

<!-- end_excerpt -->

이는 모델 공간상 정의 된 버텍스 노멀을 텍스쳐 공간으로

균일한 방법을 통해 인코딩하고 다시 디코딩하는 수학적 수식이라서

다소 추상적이다.