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

스택 오버플로우가 발생 안하고 무한히 실행 할 수 있게 해준다.

어떻게 가능하지?

리턴 스택과 몇번째 호출인지만 알면 매번 스택 정보를 쌓지 않아도 가능하다.

다만 가능하려면 한가지 규칙이 필요한데 재귀 함수 호출이 구문의 가장 마지막에 위치해야 한다.

좀 생각해보면 당연한게 만약 재귀 함수 뒤에 상태 변경 코드가 있다면 

각 재귀 함수마다 변경 된 상태를 따로 보관해야 해서 리턴 스택과 호출 회수 정보만으로 

불가능 하기 때문 일 것이다.


## release upgrade test with TCO

erlang에는 release upgrade라는 runtime 코드 교체 기능을 제공하고 있다.






























