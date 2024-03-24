---
layout: post
title: "Return Value Optimization in C++"
categories: c++
---

## Background

```c++
int[] foo();
```

<!-- begin_excerpt -->
c++에서 함수 반환 값 타입으로 `int[]` 같은 배열 타입은 지정할 수 없다. 

<!-- end_excerpt -->

```c++
void foo()
{
    int i[];
    ...
}

```

한편, 배열을 선언할 때 위와 같이 크기를 지정 안 하면 오류가 발생하는데, <br>
이는 컴파일러가 함수 호출 시 로컬 변수의 크기를 알 수 없어 얼마 만큼의 스택 공간을 미리 할당해야 할지 몰라서 발생하는 오류이다. 즉, 컴파일 타임에 배열의 크기는 알려져 있어야 한다.

> <font size="2"> 
> 동적 배열을 사용하면 컴파일 타임에 배열의 크기를 몰라도 된다. 
> 동적 배열은 런타임에 스택이 아닌 힙에 할당되고 그 주소를 컴파일 타임에 잡은 스택에서 가리킨다.
> </font>

함수의 반환 값 타입으로 `int[]`가 안되는 이유는 `int i[];` 오류와 같은 이유로 컴파일 타임에 스택의 크기를 정할 수 없기 때문이다.

다시 말해, 컴파일 타임에 함수의 반환 값이 정확히 얼마의 크기인지 알려져 있어야 한다.

## RVO

RVO는 Callee의 반환 값을 받기 위해 Caller의 스택에 반환 값의 크기만큼 할당된 공간에 값을 효율적으로 전달하는 방법에 관한 것이다.

구체적으로 컴파일러는 아래 1, 2, 3의 우선순위를 가지고 그중 하나를 선택한다.

1. Copy elision
2. Implicit move
3. Traditional copy

> <font size="2"> 
> 컴파일러는 1,2번 둘 다 할 수 없으면 3번 복사를 선택한다.
> </font>

## Copy elision

```c++
using namespace std;

string f()
{
    string a = "A";
    int b = 23;
    ...
    return a;
}

void g()
{
    string x = f();
}
```

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/RVO1.png" width="100%">
  <figcaption></figcaption>
</div>
</figure>

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/RVO2.png" width="100%">
  <figcaption></figcaption>
</div>
</figure>




...