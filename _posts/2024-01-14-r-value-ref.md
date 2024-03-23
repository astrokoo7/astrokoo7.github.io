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
    int z = add(x, y);
    return 0;
}
```

먼저 위 코드에서 이름이 있는 l value를 찾아보자. <br> 
l value 조건은 이름을 통해 값을 읽고 쓸 수 있어야 한다. <br> 
따라서 이름이 있어 이름으로 변수를 읽고 쓸 수 있는 x, y, z, a, b가 l value이다.

```
z = x + y;
```
위와 같이 l value는 이름 x와 y를 통해 값을 읽고 이름 z에 값을 쓸 수 있다.
l value는 프로그램을 짜면 당연시되어 사용하는 부분이다. 그렇다면 처음 코드에서 r value는 어떤 것일까?

5, 10, 0 그리고 하나가 숨어있다.

우선, 5와 10 그리고 0은 [코드 영역에 박혀있는 값(Literal)](https://en.wikipedia.org/wiki/Literal_(computer_programming))으로 이름이 없는 값 자체, 즉 r value다. 덧붙여, 값은 있고 이름이 없어 재사용이 불가능한 `임시 변수`이다.

> <font size="2"> 
> 코드 영역에 박혀있으니 당연히 고유의 주소도 없다.
> </font>

한편, 숨어있는 r value를 찾으려면 스택 프레임(Stack Frame)에 대한 이해가 필요하다.

함수를 호출할 때,

Callee의 반환 값을 받기 위한 공간을 Caller의 스택에 마련하고, <br>
함수 처리 후 Caller의 실행 스택으로 돌아가기 위해 현재 Caller 스택 주소를 저장한 뒤, <br>함수 인자를 Callee 스택에 쌓고 함수를 실행한다.


Caller는 반환 값을 돌려받기 위해 자신의 스택에 메모리 공간을 미리 확보하고 Callee를 호출하는데, Callee는 반환 값을 임시 변수로 생성한 뒤 Caller가 반환 값을 받기 위해 확보한 공간으로 복사하여 함수 결과를 전달한다.

> <font size="3"> 
> 위 예제는 컴파일러가 RVO(return value optimization)를 하여 임시 변수를 생성하지 않지만,  <br>
> r value를 설명하기 위해 RVO를 안 했다는 가정으로 썼다.
> </font>


```
int GetValue() {
    int a = 10;
    return a;
}

int main() {
    GetValue() = 20;
    return 0;
}
```
예를 들어, 위와 같이 작성하고 컴파일하면 

```
error: lvalue required as left operand of assignment
add() = 10;
```
이런 에러를 컴파일러가 출력하는데, 이는 왼쪽 값이 l value가 아니란 오류이다. 

> <font size="2"> 
> 컴파일러가 RVO를 하여 r value를 생성 안 하는 상황이라도 r value라고 먼저 가정하고 rule 체크하는 부분을 엿볼 수 있다.
> </font>

```
int a = 20;
10 = a;

error: lvalue required as left operand of assignment
10 = a;
```
r value(Literal)에 값을 할당하려고 하면 함수 반환 값(r value)에 값을 할당할 때와 같은 에러를 출력한다.


```
int& GetValue() {
    static int value = 10;
    return value;
}

int main() {
    GetValue() = 20;
    return 0;
}
```
참고로, 이런 식으로 함수 반환 값을 l value reference로 만들어 우회하면 함수 반환 값(l value)에도 값을 쓸 수 있긴 하다.

정리하면, 함수 반환 값도 이름이 없어 재사용이 불가능한 임시 변수인 r value이고

```
int x = 5;
int y = 10;
int z = add(x, y);
```

따라서, 위 명령어의 x,y,z는 l value이고 5, 10 그리고 함수 add(x, y)의 반환 값은 r value가 된다.

## R value reference

[l value reference](../../../../language/2023/05/27/c++-ref-pointer.html)와 마찬가지로 r value reference도 r value에 대한 reference다.

```
int a = 10;
int& b = a; // l value reference

int&& c = 10; // r value reference는 && 표기법을 사용한다.
``` 



## Const L value Reference


