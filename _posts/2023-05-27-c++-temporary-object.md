---
layout: post
title: "C++ Temporary Object"
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

메모리상 값 복사는 register를 통해 이뤄지는데 

참조 타입에 x를 할당하는 부분은

lea 명령어로 rbp(base pointer)로부터 20만큼 뺀 주소를 rax로 저장한 뒤  
mov 명령어를 통해 rax 주소가 가리키는 값을 rbp로부터 16만큼 뺀 주소로 복사하고 있다

마찬가지로 포인터에 x가 가리키는 값도 위와 동일하게 [rbp-20]의 값을 [rbp-16]로   
복사하고 있는 부분을 볼 수 있다.

컴파일러는 참조 타입을 사용하여 포인터의 취약점을 보완하면서도 동일한 동작을   
수행하는 명령을 생성한다.


## move semantics


