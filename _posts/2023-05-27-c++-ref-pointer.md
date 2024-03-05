---
layout: post
title: "C++ ref vs pointer"
categories: language
---

```c++
int main() {
    int x = 10; 
    int& ref = x; 
    int* p = &x;
    return 0;
}
```

c++의 참조 타입과 포인터를 어셈블리 코드로 보면 똑같다

```
main:
        push    rbp
        mov     rbp, rsp
        mov     DWORD PTR [rbp-20], 10
        lea     rax, [rbp-20]
        mov     QWORD PTR [rbp-8], rax
        lea     rax, [rbp-20]
        mov     QWORD PTR [rbp-16], rax
        mov     eax, 0
        pop     rbp
        ret
```

`int& ref = x;`  에 해당하는 코드는 아래와 같고 

```
lea     rax, [rbp-20]
mov     QWORD PTR [rbp-8], rax
```

`int* p = &x;` 에 해당하는 코드는 아래와 같다

```
lea     rax, [rbp-20]
mov     QWORD PTR [rbp-16], rax
```

<!-- begin_excerpt -->

보통 어떤 메모리의 값 변경은 변경하려는 값의 주소를 cpu register로 지정한 뒤 register가 가리키는 주소의 값을 메모리에 써 줌으로서 변경한다.

<!-- end_excerpt -->

참조 타입에 값을 할당시 lea 명령어를 사용해 값의 주소를 register에 지정하고 mov 명령어를 사용해 register가 가리키는 주소의 값으로 써주는 식이다.

참조와 동일하게 포인터 x의 값 할당도 lea 명령어로 값의 주소를 register에 지정하고  레지스터가 가리키는 주소의 값을 mov 명령어를 통해 메모리에 써준다.  
  
  
참조 타입은 c++ 언어 레벨의 포인터의 취약점을 보완한 개념으로 기계어 레벨에서는 포인터나 참조나 동일하게 동작함을 엿볼 수 있다.


