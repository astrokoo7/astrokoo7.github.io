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
이런 식으로 l value는 이름 x와 y를 통해 값을 읽고 이름 z에 값을 쓸 수 있다.

l value는 프로그램을 짜면 당연하게 사용한 부분이다. 그렇다면 처음 코드에서 r value는 어떤 것일까?

5, 10, 0 그리고 하나가 숨어있다.

우선, 5와 10 그리고 0은 코드 영역에 박혀있는 값(value)으로 이름이 없는 값 자체로 r value이다. 즉, 값은 있는데 이름이 없어 재사용이 불가능한 임시 변수이다.


## L-Value Reference

lvalue는 값을 가리키고 lvalue reference는 값의 주소를 가리킨다.<br> 
또한, [앞선 글](../../../../language/2023/05/27/c++-ref-pointer.html)에서 lvalue  reference와 pointer는 기계어 수준에선 동일하게 동작한다고 이야기했었다.

lvalue reference는 pointer의 주소 연산 같은 취약점을 개선하고 장점인 stack frame을 벗어나 scope에 구애받지 않는 값 참조가 가능하다.

컴파일러는 함수 호출 전 함수 인자를 stack frame에 저장한 뒤 함수를 호출하는데 이때 인자 타입이 lvalue reference이면 stack frame에 값을 가리키는 메모리 주소만 할당한다. 반면 인자 타입이 lvalue 라면 stack frame에 값을 복사한다.

## R-Value Reference

r value는 이름이 없는 값 그 자체라고 이야기했었다. <br>
이는 값에 대한 주소 공간에 없단 말과 같고 코드에 상수처럼 박힌 채로 존재한다. <br>

한편, reference는 값에 대한 주소 공간을 가리킨다. <br>
그런데 r value reference는 코드에 박혀있어 주소가 없는 r value를 reference 할 수 있을까?

컴파일러가 내부에서 임시로 l value를 만들고 그 l value의 주소를 r value reference가 가리키게 만든다.

이쯤 되면 l value reference와 r value reference가 그럼 같은 것 아닌지 혼란스러워질 수 있는데, 중요한 건 기계어로 표현되는 방식이 같아도 c++ 언어 레벨에선 l value와 r value는 분명히 개념적으로 다르다. 단지, lvalue reference와 Pointer가 그랬듯 결과물인 기계어는 같아도 컴파일러는 둘을 명확히 구분하여 처리하고 있다.




<!-- 
r value는 메모리 공간 자체가 원래는 없다.
-->


## Identifying L value and R value using Reference

```
int main() {
    std::string firstName = "foo";
    std::string secondName = "bar";
    std::string fullName = firstName + secondName;
}
```

## Const L value Reference
