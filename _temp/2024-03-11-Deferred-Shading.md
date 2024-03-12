---
layout: post
title: "Deferred Shading"
categories: graphics
---

## overview

forward rendering의 경우 랜더링시 광원 개수 * 오브젝트 개수 만큼 비용이 비싼 pixel shader에서 

Deferred Shading은 기존 forward rendering에서 geometry 연산과 light 연산의 분리

geometry 연산 시 depth test를 통과한 최종 픽셀만 gbuffer에 남아서 light 연산을 통한 shading 시 blending을 할 수 없음
             
forward rendering에서 할 수 없는 light의 감쇠로 영향 받지 않아도 되는 pixel을 계산에서 제외 할 수 있음

영향 받지 않는 pixel을 계산에서 제외시 거리 계산을 통한 if branch 보다 실제 빛의 영향을 받는 최대 거리를 계산해 스피어를 실제 그림으로서 if branch 없이 영양 받는 pixel 만 light 계산을 하는게 더 좋음

-> if 문을 어떻게 제거하는지
-> 스피어를 그리면 resterize 되기 전에 screen space상에서 pixel 연산을 함 
-> 그럼 스피어 밖에 있는 pixel도 계산함


비용이 씨간 light 연산을 geometry 연산에서 분리하여 n*m 에서 n+m 연산으로 최적화

그에따른 제약 -> 부분 foward rendering

geometry 연산 분리를 통한 추가 장점 -> 데이터를 활용한 연산 최적화
light volume

MSAA를 할 수 없음

