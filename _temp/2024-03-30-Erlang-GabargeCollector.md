---
layout: post
title: "Erlang Garbage Collector"
categories: erlang
---

c/c++처럼 erlang도 stack과 heap을 구조를 사용해 프로그램을 실행한다. 하지만 각각을 사용하는 방법이 다르다.

erlang은 Garbage Collector를 사용해 stack과 heap을 대신 관리해주는데 그 방법이 C#과도 다르다.

모든 변수는 처음엔 모두 stack에 쌓인다.  



