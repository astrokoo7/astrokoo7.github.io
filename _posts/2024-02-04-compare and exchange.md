---
layout: post
title: "Lock Free"
categories: c#
---

```c#
private static int bar = 0;

public static void AddOne() {
    bar += 1;
}
```

위 코드에서 정적(static) 변수 bar는 

정적 함수 AddOne의 호출을 통해 갱신되는데 구체적으론 다음의 단계를 거친다.

```
1. bar의 값을 메모리에서 읽어와
2. 읽은 값에 +1를 더한뒤
3. 그 결과를 bar의 메모리에 저장한다.
```

<!-- begin_excerpt -->

즉 단 한줄의 명령어 `bar += 1;` 는 읽기와 쓰기가 atomic 하게 동작하지 않는다.

<!-- end_excerpt -->

만약 멀티 쓰레드 환경에서 동시에 여러개의 쓰레드가 AddOne을 호출한다고 생각해보자.

위 단계 1,2,3은 각자의 쓰레드에서 같은 시간에 실행 될 수 있다.

예를들어 bar의 값이 0일때 두개의 쓰레드 A,B가 동시에 1,2,3을 실행한다면

```
1. A, B가 1을 실행하여 각자 bar 값 0을 읽어옴
2. A, B가 각자 읽은 값에 +1을 더함
3. A, B가 결과값 1을 bar에 메모리에 저장
```










