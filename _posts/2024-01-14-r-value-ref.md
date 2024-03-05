---
layout: post
title: "R-Value Reference"
categories: c++
---

<!-- begin_excerpt -->

l value 는 프로그래머가 이름을 지을 수 있는 반면

r value 는 r value reference 개념이 나오기 전엔 

프로그래머가 이름을 지어 사용할 수 없었다.

<!-- end_excerpt -->

이름을 지어 사용할 수 없었던 r value는 그럼 그동안 눈으로 볼 수 있었을까?

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

위 코드에서 r value는 몇개나 있을까?

hint는 = 인 할당 연산자에서 찾을 수 있다.

할당 연산자는 우측 값을 좌측 값에 할당해주는 연산자이다. 

명령어 `int x = 5;` 에서 우측 값 5를 좌측 변수의 값에 할당했다.

여기서 주목해야 할 점은 5란 값으로 변수 이름이 없는 값 자체로 재사용 불가능한 임시 변수다. 

이런식으로 값에 이름이 없는 변수를 r value라 부르고 이름이 없기 때문에 프로그래머가 재사용을 할 수 없는 특징이 있었다.


## L-Value Reference

rvalue reference를 살펴보기 전에 lvalue reference를 먼저 짚고 넘어가자.

> <font size="2"> 
> lvalue reference를 이해해야 rvalue reference를 왜 만들었는지 이해할 수 있다.
> </font>

앞선 [글](../../../../language/2023/05/27/c++-ref-pointer.html)에서 lvalue reference와 pointer는 기계어 수준에선 동일하게 동작한다고 이야기했었다.

...

<!-- 함수 스택과 연관성을 지어 설명 -->



## Copy Constructor

in progress ..


