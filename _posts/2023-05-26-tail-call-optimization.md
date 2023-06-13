---
layout: post
title: "Tail Call Optimization"
categories: server
---


```elixir
defmodule Foo do
    def bar do
        bar()
    end
end
```

elixir에서 Foo라는 모듈의 bar 함수를 호출하면 스스로 bar를 호출하여 무한 루프에 빠지는데

TCO를 통해 스택 오버플로우가 발생 안하고 무한히 실행 할 수 있게 해준다.

## how?

리턴 스택 주소와 몇번째 호출인지만 알면 매번 스택 정보를 쌓지 않아도 가능하다.

다만 가능하려면 한가지 규칙이 필요한데 재귀 함수 호출이 구문의 가장 마지막에 위치해야 한다.

좀 생각해보면 당연한게 만약 재귀 함수 뒤에 상태 변경 코드가 있다면 

각 재귀 함수마다 변경 된 상태를 따로 보관해야 해서 리턴 스택 주소와 호출 횟수 정보만으로 

불가능 하기 때문 일 것이다.


## release upgrade test with TCO

erlang에는 release upgrade라는 runtime 코드 교체 기능을 제공하고 있다.

관련 문서를 보면 같은 코드를 old와 new인 상태로 동시에 vm 상에서 동작 가능하다고 한다.

예를들어 A 코드를 교체 할때 이미 A 코드에 진입해서 실행 중인 프로세스가 있어도 

vm은 old 코드로 취급해서 실행 중인 코드가 이상 없이 마무리 되도록 해준다.

여기까진 좋다. 하지만 그 뒤에 따라오는 궁금함이 많다.

예를들어 old A가 끝나면 A가 B를 호출하는데 그 B도 new B라면 old B를 호출할지 new B를 

호출할지 같은 것 말이다. 

문서에는 모듈 의존 관계에 대해 명시하고 있는 부분이 있다.

A가 caller이고 B가 callee라면 B를 먼저 교체하고 A를 교체하는 식의 내용이다. 

하지만 이건 new A를 진입했다면 new B가 호출 되야 한다는 상식적인 인식 수준이다.

그럼 다시 돌아가 이미 실행 중인 old A가 B를 호출 할때 old B를 호출할지 new B를 호출

할지 어떻게 확인하고 검증할지 vm 코드를 다 분석하기엔 너무 깊다.

TCO를 사용해서 원하는 답을 얻을 수 있었다.

결론은 old A는 new B를 호출하지만 B의 signiture가 같다면 호출하는데 문제 없고 

다르면 호출하려는 함수를 찾지 못했다 이다.



- [ref](https://learnyousomeerlang.com/relups#the-hiccups-of-appups-and-relups)

















































