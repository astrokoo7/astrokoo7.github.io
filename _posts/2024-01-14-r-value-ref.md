---
layout: post
title: "R-Value Reference"
categories: c++
---

## Overview

<!-- begin_excerpt -->

l value 는 프로그래머가 이름을 지을 수 있는 반면 <br>
r value 는 r value reference 개념이 나오기 전엔 <br> 
프로그래머가 이름을 지어 사용할 수 없었다.<br> 

<!-- end_excerpt -->

이름을 지어 사용할 수 없다는 말의 의미는 무었일까?

```
int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 5;
    int y = 10;

    int additionResult = add(x, y);
}
```

위 코드에서 r value는 몇개나 있을까? <br> 
hint는 = 인 할당 연산자에서 찾을 수 있다. <br> 
할당 연산자는 우측 값을 좌측 값에 할당해주는 연산자이다. <br> 

명령어 `int x = 5;` 에서 우측 값 5를 좌측 변수의 값에 할당했다. <br> 
여기서 주목해야 할 점은 5란 값으로 변수 이름이 없는 값 자체로 재사용 불가능한 임시 변수다. <br> 

이런식으로 값에 이름이 없는 변수를 r value라 부르고 이름이 없기 때문에 <br> 프로그래머가 재사용을 할 수 없는 특징이 있었다.

<!-- 

heap vs stack frame 
그래서 논조가 임시 변수의 활용
lvalue reference도 임시 변수를 활용하는건 아니지만 자원의 활용
바운더리를 넘어선 (stack frame) 값을 읽을 수 있다.
하지만 lvalue ref도 자신의 소유권을 넘겨주진 못한다.
fire and forget 상황에서 적절하겠어
-->

## L-Value Reference

rvalue reference를 살펴보기 전에 lvalue reference를 먼저 짚고 넘어가자.

[앞선 글](../../../../language/2023/05/27/c++-ref-pointer.html)에서 lvalue  reference와 pointer는 기계어 수준에선 동일하게 동작한다고 이야기했었다.

lvalue reference는 pointer의 주소 연산 같은 취약점을 개선하고 장점인 stack frame을 벗어나 scope에 구애받지 않는 값 참조가 가능한다.

컴파일러는 함수 호출 전 함수 인자를 stack frame에 저장한 뒤 함수를 호출하는데 이때 인자 타입이 lvalue reference이면 stack frame에 값을 가리키는 메모리 주소만 할당한다. 반면 인자 타입이 lvalue reference가 아니면 stack frame에 값을 복사한다.





...


