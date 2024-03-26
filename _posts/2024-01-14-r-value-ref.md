---
layout: post
title: "Rvalue Reference"
categories: c++
---

## Rvalue

<!-- begin_excerpt -->

lvalue 는 프로그래머가 이름을 지을 수 있는 반면 <br>
rvalue 는 rvalue reference 개념이 나오기 전엔 <br> 
프로그래머가 이름을 지어 사용할 수 없었다.<br> 

<!-- end_excerpt -->

이름을 지어 사용할 수 없다는 말의 의미는 무었일까?

```c++
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

먼저 위 코드에서 이름이 있는 lvalue를 찾아보자. <br> 
lvalue 조건은 이름을 통해 값을 읽고 쓸 수 있어야 한다. <br> 
따라서 이름이 있어 이름으로 변수를 읽고 쓸 수 있는 x, y, z, a, b가 lvalue이다.

```c++
z = x + y;
```
위와 같이 lvalue는 이름 x와 y를 통해 값을 읽고 이름 z에 값을 쓸 수 있다.
lvalue는 프로그램을 짜면 흔하게 사용하는 부분이다. 그렇다면 위 코드에서 rvalue는 어떤 것일까?

5, 10, 0 그리고 보이지 않는 하나가 숨어있다.

우선, 5와 10 그리고 0은 [코드 영역에 박혀있는 값(Literal)](https://en.wikipedia.org/wiki/Literal_(computer_programming))으로 이름이 없는 값 자체, 즉 rvalue다. 덧붙여, 값은 있고 이름이 없어 재사용이 불가능한 `임시 변수`이다.

> <font size="2"> 
> 코드 영역에 박혀있으니 당연히 고유의 주소도 없다.
> </font>

한편, 숨어있는 rvalue를 찾으려면 저수준의 함수 실행 과정에 대한 이해가 필요하다.

함수를 호출할 때,

Callee의 반환 값을 받기 위한 공간을 Caller의 스택에 마련하고, <br>
함수 처리 후 Caller의 실행 스택으로 돌아가기 위해 현재 Caller 스택 주소를 저장한 뒤, <br>함수 인자를 Callee 스택에 쌓고 함수를 실행한다.

이때, <br>
Callee는 반환 값을 Caller에 돌려주기 위해, <br>
반환 값에 대한 임시 변수를 생성한 뒤, <br>
Caller가 반환 값을 받기 위해 확보한 공간에 임시 변수를 복사하여, <br>
Caller에게 함수 결과를 전달한다.

> <font size="3"> 
> 위 예제는 컴파일러가 RVO(return value optimization)를 하여 임시 변수를 생성하지 않지만,  <br>
> rvalue를 설명하기 위해 RVO를 안 했다는 가정으로 썼다.
> </font>


```c++
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

```c++
error: lvalue required as left operand of assignment
add() = 10;
```
이런 에러를 컴파일러가 출력하는데, 이는 할당 연산자 왼쪽의 값이 lvalue가 아니란 오류이다. 

> <font size="2"> 
> 컴파일러가 RVO를 하여 rvalue를 생성 안 하는 상황이라도 rvalue라고 먼저 가정하고 rule 체크하는 부분을 엿볼 수 있다.
> </font>

```c++
int a = 20;
10 = a;

error: lvalue required as left operand of assignment
10 = a;
```
명시적인 rvalue(Literal)에 값을 할당하려고 하면 함수 반환 값(rvalue)에 값을 할당할 때와 같은 에러를 출력한다.


```c++
int& GetValue() {
    static int value = 10;
    return value;
}

int main() {
    GetValue() = 20;
    return 0;
}
```
참고로, 이런 식으로 함수 반환 값을 lvalue reference로 만들어 우회하면 lvalue 함수 반환 값에도 값을 쓸 수 있긴 하다.

정리하면, 함수의 반환 값 예시처럼 rvalue는 컴파일러가 특정 상황에 필요에 의해 생성하는 임시적인 변수 또는 Literal을 말한다. 

> <font size="2"> 
> 임시 변수는 C++11의 이동 시맨틱인 std::move가 소개되면서 임시 변수의 값 복사 대신 소유권을 넘길 수 있게 되었다.
> </font>


## Rvalue Reference

[lvalue reference](../../../../language/2023/05/27/c++-ref-pointer.html)와 마찬가지로 rvalue reference도 rvalue에 대한 reference다.

```
int a = 10;
int& b = a;   // lvalue reference
int&& c = 10; // rvalue reference는 && 표기법을 사용한다.
``` 

한편, rvalue reference는 특이한 특성이 있는데 rvalue를 가리키지만 rvalue reference는 lvalue이다. <br>
이를 확인할 수 있는 직관적인 룰이 있는데 다음과 같다.

1. lvalue reference는 lvalue만 reference 할 수 있다.<br>
2. rvalue reference는 rvalue만 reference 할 수 있다.

```
void foo(int& a) {}

void bar()
{
    int a = 10;
    foo(a);      // lvalue를 lvalue reference에 할당해서 문제없다. 
    foo(10);     // rvalue를 lvalue reference에 할당하려 해서 컴파일 오류가 발생한다.
}
```

```
void foo(int&& a) {}

void bar()
{
    int a = 10;
    foo(a);      // lvalue를 rvalue reference에 할당해서 해서 컴파일 오류가 발생한다.
    foo(10);     // rvalue를 rvalue reference에 할당해서 문제없다. 
}
```

이때, rvalue reference는 위 조건에 따라 lvalue reference에 대입하면 문제가 없고 rvalue reference에 대입하면 컴파일 오류가 발생한다.

```
void foo(int& a) {}

void bar()
{
    int&& a = 10;
    foo(a);     // rvalue reference는 lvalue라서 컴파일 오류가 발생 안 한다.
}
```

```
void foo(int&& a) {}

void bar()
{
    int&& a = 10;
    foo(a);     // rvalue reference는 lvalue라서 rvalue reference에 대입하면 오류가 발생한다.
}
```



## Const Lvalue Reference


