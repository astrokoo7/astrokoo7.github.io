---
layout: post
title: "Lock Free"
categories: c#
---

```c#
private static int bar = 0;

public static void AddOne() {
    bar += 1;
}
```

위 코드에서 정적(static) 변수 bar는 

정적 함수 AddOne의 호출을 통해 갱신되는데 구체적으론 다음의 단계를 거친다.

```
1. bar의 값을 메모리에서 읽어와
2. 읽은 값에 +1를 더한뒤
3. 그 결과를 bar의 메모리에 저장한다.
```

<!-- begin_excerpt -->

즉 단 한줄의 명령어 `bar += 1;` 는 읽기와 쓰기가 atomic 하게 동작하지 않는다.

<!-- end_excerpt -->

만약 멀티 쓰레드 환경에서 동시에 여러개의 쓰레드가 AddOne을 호출한다고 생각해보자.

위 단계 1,2,3은 각자의 쓰레드에서 같은 시간에 실행 될 수 있다.

예를들어 bar의 값이 0일때 두개의 쓰레드 A,B가 동시에 1,2,3을 실행한다면 다음과 같고

```
a. A, B가 1을 실행하여 각자 bar 값 0을 읽어옴
b. A, B가 각자 읽은 값에 +1을 더함
c. A, B가 결과값 1을 bar의 메모리에 저장
```

결과적으로 A,B 쓰레드가 각각 AddOne을 호출하여 bar를 두번 +1 했음에도 

값은 2가 아니라 1이 됬다.

멀티 쓰레드 환경에선 위와 같이 공유 자원에 대한 경쟁 상태(race condition)가 

발생하는데 이를 방지하기 위해 여러가지 상호 배제(mutual exclusion) 기술이 존재한다. 

<br>
전형적인 상호 배제 기술로 lock이 있는데 lock은 어떤 쓰레드가 공유 자원 사용을 위해 

lock 획득 시도시 이미 다른 쓰레드에 의해 lock 되어 사용중이라면 그 즉시 해당 쓰레드는 

sleep 상태로 전환된다.

<br>
이는 해당 쓰레드가 스케줄러로부터 할당 받은 시간 할당량(time slice)이 아직 남아있어도 

즉시 포기하게 되며 다른 대기중인 스레드로 제어권이 넘어가면서 context switching 

비용도 발생하게된다.

<br>
일반적으로 공유 자원에 대한 긴 대기 시간을 가지는 hard disk io 작업 같은 경우 위와 

같은 lock 동작은 적합한 상호 배제(mutual exclusion) 기술이다.

<br>
하지만 만약 공유 자원 사용 시간이 매우 짧다면 그 작은 시간을 기다리기 위해 남은 시간 

할당량(time slice)을 즉시 포기해야하고 그로인해 비용이 큰 context swithcing도 

발생하게 된다. 이는 다수의 쓰레드가 빈번하게 이러한 경쟁 상태에 놓일시 병목 현상

(bottle neck)의 주원인이 될 수 있다.

## Busy Wait

매우 짧은 시간만 필요하여 금방 해제 될 공유 자원을 기다려야 할 경우 쓰레드가 대기 상태

(sleep)로 빠져 시간 할당량을 포기하기 보단 활성화 상태를 유지하여 공유 자원을 쓸수 

있는지 계속 확인하는 편이 낮다.

```c#
public class ThreadSafe
{
    private float totalValue = 0.0F;
    public float Total { get { return totalValue; }}

     public float AddToTotal(float addend)
    {
        float initialValue, computedValue;
        do 
        {
            initialValue = totalValue;
            computedValue = initialValue + addend;
        } while (initialValue != 
               Interlocked.CompareExchange(ref totalValue, computedValue, initialValue));
        return computedValue;
    }
}
```

위 코드는 Compare And Exchange 기법을 통해 Busy Wait을 구현하였는데 간략하게 

풀어쓰면 이렇다.

```
1. 공유 자원 totalValue를 메모리에서 읽어와 임시 변수에 저장 후
2. 임시 변수 값에 인자를 더한뒤
3. 공유 자원 totalValue의 메모리에 임시 변수 값으로 저장한다.
```

이는 처음 보았던 `bar += 1;`과 똑같은 단계를 거치는걸 알 수 있다. 

<br>
하지만 다른점은 CompareExchange 함수를 사용한 부분으로 마찬가지로 다른 쓰레드가 공유 

자원을 동시에 읽은 뒤 변경할 수 있지만 변경점을 메모리에 쓸때 애초에 읽은 값이 다른 

쓰레드에 의해 변경 된 경우라면 이를 감지할 수 있어 처음부터 읽고 쓰기를 다시 시도할 수 

있게 된 점이다.

<br>
즉 CompareExchange 함수는 공유자원을 메모리에서 읽은 시점의 snap shot인 

initialValue와 공유 자원인 totalValue를 비교하여 같다면 다른 쓰레드의

변경이 없다고 판단하여 totalValue를 computedValue로 갱신해 준다.

## Spin Wait  

앞서 보았듯 busy wait은 매우 짧은 시간 동안 공유 자원을 사용할 때 적합하다.

하지만 busy wait에서 사용한 빈 루프(empty loop)를 좀 더 여러 상황에 따라 사용할 수 

있도록 일반화가 필요하다.

<br>
예를들어 만약 core가 단하나인 cpu의 경우 empty loop만 사용한다면 다른 모든 쓰레드가 

무한 대기에 빠질 수 있다. 또한 멀티 core 머신에서도 우선 순위가 더 높은 쓰레드가 

blocking 되는걸 방지한다거나 garbage collector에게 양보하기 위한 고려가 되있어야 

한다. 이렇듯 상황에 따라 충분히 busy wait 하였다면 스스로 제어권을 포기하기 위한 

방법도 제공되어야 한다.

<br>
c#에선 `System.Threading.SpinWait`을 사용하여 busy wait을 좀 더 안전하고 범용적으로 

구현 할 수 있다.

```c# 
public class LockFreeStack<T>
{
    private volatile Node m_head;

    private class Node { public Node Next; public T Value; }

    public void Push(T item)
    {
        var spin = new SpinWait();
        Node node = new Node { Value = item }, head;
        while (true)
        {
            head = m_head;
            node.Next = head;
            if (Interlocked.CompareExchange(ref m_head, node, head) == head) break;
            spin.SpinOnce();
        }
    }

    public bool TryPop(out T result)
    {
        result = default(T);
        var spin = new SpinWait();

        Node head;
        while (true)
        {
            head = m_head;
            if (head == null) return false;
            if (Interlocked.CompareExchange(ref m_head, head.Next, head) == head)
            {
                result = head.Value;
                return true;
            }
            spin.SpinOnce();
        }
    }
}
```

위 예제에서 linked list를 쓰레드간 공유하고 싶은 경우 head위치 갱신만 동기화 함으로서 

멀티 쓰레드 환경에서 어떻게 빠르고 효율적으로 데이터 자료 구조를 공유하는지 살펴 볼 수 

있다.

<br>
하지만 단순 index 뿐 아니라 좀더 복잡한 구조의 자료구조의 경우 mutex lock을 

사용하는게 좀더 안전한 구현도 될수 있으니 lock free 기법을 사용할 때는 여러가지 주의 

사항과 필요성을 고려해 사용해야 한다.











