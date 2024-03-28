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

```c++
struct Foo
{
    char a[10];
};

struct Bar : Foo
{
    char a[10];
};

Foo hello()
{
    Bar b = Bar();
    return b;
}

int main()
{
    Foo foo = hello();
}
```
참고로, 위 코드는 에러가 안 나기 때문에 Caller가 반환 값 크기를 모를 수 있지 않냐고 반문할 수 있다. 하지만 main에서 받은 foo는 Bar type이 아니고 Bar type 중 잘린 Foo type이다. 이를 [Object slicing](https://en.wikipedia.org/wiki/Object_slicing)이라고 부른다. 마찬가지로 컴파일 타임에 반환 값 크기는 정해져 있다. 

> <font size="2"> 
> 함수 종료와 함께 callee의 스택이 전부 Pop 되는 걸 생각하면 이 모든 동작이 자연스럽다.
> </font>

## Return Value Optimization

RVO는 Caller의 스택 중 Callee의 반환 값을 받기 위해 반환 값의 크기만큼 할당된 메모리 공간에 Callee가 반환 값을 효율적으로 전달하는 방법에 관한 것이다.

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
위 코드를 예로 copy elision이 어떻게 동작하는지 아래 그림을 통해 확인해 보자

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/RVO1.png" width="100%">
  <figcaption>copy elision이 아닐 땐, Callee의 반환 값 로컬 변수 a를 Caller의 로컬 변수 x로 복사한다.</figcaption>
</div>
</figure>

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/RVO2.png" width="100%">
  <figcaption>copy elision 일 땐, Callee의 반환 값 로컬 변수 a를 Caller의 로컬 변수 x의 저장 공간에 직접 썼다.</figcaption>
</div>
</figure>


## when can't we elide?

```c++
Fruit apples_to_apples(int i, Fruit x, int j)
{
    ...
    return x;
}
```
위와 같이 x가 어디서 생성되었는지 알 수 없을 때, 반환 값을 Caller의 스택 영역에 바로 쓸 수 없어 Copy elision은 작동하지 않는다. x와 Caller가 받을 반환 값은 공유할 수 없는 자원이고 서로 다른 life cycle을 가지기 때문이다.

```c++
Fruit apples_and_oranges()
{
    Foo x = ...;
    Foo y = ...;
    ...
    if (evaluate(x, y)) {
        return x;
    } else {
        return y;
    }
}
```
컴파일 타임에 반환 값을 결정할 수 없을 때도 Copy elision은 작동하지 않는다. 컴파일러는 미리 정해진 반환 값을 Caller의 스택 영역에 쓰도록 코드를 생성하는데 반환 값을 모른다면 Copy elision을 할 수 없기 때문이다.

## Copy elision is observable

```
string* gp;

string f()
{
    std::string a = "A";
    gp = &a;  // 반환 값의 로컬 변수 주소를 글로벌 변수에 저장해 둔다.
    return a;
}

int main()
{
    std::string x = f();

    // Caller의 로컬 변수 x의 주소와 Callee의 반환 값이
    // 같아 Copy elision이 된 걸 확인 할 수 있다.
    assert(&x == gp); 
}
```


## Implicit move is also observable

```
std::string* s;

std::string factory(std::string name)
{
    s = &name;
    return name;
}

int main()
{
    auto a = factory(std::string{ "hello" });

    // 로컬 변수 a의 주소와 Callee의 로컬 변수 name의 주소가 다르다.
    // 이는 copy elision이 되지 않았음을 알 수 있다. 
    assert(&a != s); 

    // 아래의 조건문으로 Callee의 반환 값이 Caller의 로컬 변수로
    // 암묵적 이동 되었음을 알 수 있다.    
    assert(!a.empty() and s->empty());
    return 0;
}
```
앞서 이야기한 것처럼 이 경우는 copy elision이 무시되지만 컴파일러는 차선책으로 암묵적 이동을 해준다.



...