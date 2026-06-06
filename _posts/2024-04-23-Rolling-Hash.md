---
layout: post
title: "Rolling Hash"
categories: c++
---

<!-- begin_excerpt -->
어떤 문자열에서 특정 패턴을 검색할 때 brute force 방식을 쓰면, 입력 문자열 길이가 `n`, 패턴 길이가 `m`일 때 시간 복잡도는 `O(n*m)`이 된다.
<!-- end_excerpt -->

패턴과 입력이 짧을 땐 brute force로 충분하지만, 리눅스 `grep`처럼 아주 큰 문자열에서 긴 패턴을 찾을 땐 적합하지 않다. 이런 상황에선 KMP, Boyer-Moore 같은 알고리즘을 주로 쓰는데, 여기서는 Rabin-Karp으로 알려진 간단한 Rolling Hash를 살펴본다.

그전에 brute force 방식의 코드를 잠시 보고 넘어가자.

```c++
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// brute force 방식으로 긴 문자열에서 검색할 땐 적합하지 않다.
vector<int> findPatternOccurrences(const string& text, const string& pattern) {
    vector<int> occurrences;
    int n = text.size();
    int m = pattern.size();

    for (int i = 0; i <= (n - m); i++) {
        int j = 0;
        // 0~(n-m)까지 m번 검색한다.
        for (; j < m; j++) {
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m) 
            occurrences.push_back(i);
    }
    return occurrences;
}

int main() {
    string text = "aaabaaabbbaaabbaabb";
    string pattern = "aaab";

    vector<int> occurrences = findPatternOccurrences(text, pattern);
    for (const auto& occurrence : occurrences) {
        cout << occurrence << " ";
    }
    return 0;
}
```

## Rolling Hash

Rolling Hash는 입력 문자열과 패턴의 해시 값을 비교하는 방식으로 평균 `O(n+m)` 시간 복잡도를 가진다. 패턴 크기만큼 해시를 생성하는 부분과, 그 해시를 한 칸씩 이동시키며 비교하는 부분으로 나눠 생각해 보자.

- [Generate Hash](#generate-hash)
- [Sliding Hash Window](#sliding-hash-window)

### Generate Hash

해시 값은 문자열의 각 문자를 ASCII code 값으로 인코딩해서 만든다. 이때 같은 문자라도 위치가 다르면 구분되도록, 자릿수마다 base(밑)의 거듭제곱을 가중치로 곱해 더한다. base는 해시 충돌이 덜 발생할 값으로 정한다.

```
입력 문자열 aaa에 대한 hash를 생성해 보자

h(aaa) = h(a, a, a)
       = h(1, 1, 1)
       = 1 * 258^2 + 1 * 258^1 + 1 * 258^0
       = 66564 + 258 + 1
       = 66823
```
> <font size="2"> 
> 편의상 a의 ASCII code가 1이라 치고, base는 ASCII code 크기의 두 배인 258로 넉넉히 정했다.
> </font>

```
h(aaa) = h(a, a, a)
       = h(1, 1, 1)
       = (1 * 258^2 + 1 * 258^1 + 1 * 258^0) mod 1000000007
```
위에서 구한 해시 값은 문자열이 조금만 길어져도 정수형 범위를 훌쩍 넘으므로, modular 연산으로 값을 잘라 준다.

한편, modular 연산 값은 큰 소수를 사용하여 hash 충돌이 덜 발생하도록 고려되어야 한다.

> <font size="2"> 
> 만약 소수가 아닌 조합 수를 사용한다면 약수의 배수를 통해 지정 범위 보다 더 많은 hash 충돌이 발생하게 된다. 
> </font>

rolling hash 알고리즘은 hash 충돌이 많이 발생할수록 시간 복잡도가 최대 O(n*m) 만큼 증가하게 되어 효율성이 급격히 저하되는 특징이 있다. 

```c++
    if (patternHash == textHash && pattern == text.substr(0, m)) {
        occurrences.push_back(0);
    }
```
> <font size="2"> 
> 해시가 충돌하면 해시 값은 같지만 실제 문자열은 달라서 재확인 비용만 늘어난다. 애초에 충돌이 덜 발생하게 만들어야 하는 이유다. 참고로 C++의 `==` 문자열 비교 연산자는 내부적으로 brute force 방식으로 문자를 비교한다.
> </font>

위 코드는 해시 값 비교 자체는 `O(1)`이지만, `==`로 문자열을 재확인할 때 `O(m)`이 든다. 이런 특성 때문에 패턴이 짧을수록 Rolling Hash가 유리한 경향이 있다.

```
258^99 mod 1000000007 = ?
```    
문자열이 길면 modular 연산 도중에도 overflow가 발생할 수 있다. 이는 modular 연산의 분배 성질을 이용해 아래처럼 작은 수로 나눠서 계산할 수 있다.

```
(A * B) mod C = ((A mod C) * (B mod C)) mod C

258^2 mod 1000000007 = ((258 mod 1000000007) * (258 mod 1000000007)) mod 1000000007
```    

이를 코드로 옮기면 아래와 같다

```c++
    int power = 1;
    for (int i = 1; i <= 99; i++) {
        power = (power * 258) % 1000000007;
    }
```

### Sliding Hash Window

먼저 패턴 길이만큼 입력 문자열의 해시 값을 만들고 패턴 해시와 비교한다. 다르면 인덱스를 한 칸씩 밀면서 해시 값을 갱신해 다시 비교하는 식으로 일치하는 패턴을 찾는다.

이때 매번 처음부터 해시를 다시 계산하지 않고, 맨 앞 문자를 빼고 뒤 문자를 붙이는 방식으로 갱신한다. `MOD = 1000000007`, `power = base^(m-1)`(맨 앞 자리의 가중치)라고 할 때, 윈도우를 `abcd`에서 `bcde`로 한 칸 미는 과정은 다음과 같다.

```
    text: abcdefghijk

    remove : h = (h - a * power) mod MOD     // 맨 앞 문자 a의 기여를 뺀다
    shift  : h = (h * base)      mod MOD     // 남은 항을 한 자리 올린다
    append : h = (h + e)         mod MOD     // 새 문자 e를 더한다
```

복잡해 보이지만 결국 **맨 앞 값(a)을 빼고 → 한 자리 올린 뒤 → 뒤 값(e)을 더하는** 세 단계다. 이렇게 갱신한 해시를 패턴 해시와 비교해 일치하는 위치를 찾는다.

위 내용을 코드로 짜면 아래와 같다.

```c++
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int MOD = 1000000007;

int hashFunction(char c) {
    return c - 'a' + 1;
}

int calculateRollingHash(const string& s, int start, int end, int base) {
    int hash = 0;
    int power = 1;

    for (int i = end; start <= i; i--) {
        hash += (hashFunction(s[i]) * power) % MOD;
        power = (power * base) % MOD;
    }
    return hash;
}

vector<int> findPatternOccurrences(const string& text, const string& pattern) {
    vector<int> occurrences;
    int n = text.length();
    int m = pattern.length();
    int base = 256;

    int patternHash = calculateRollingHash(pattern, 0, m - 1, base);
    int textHash = calculateRollingHash(text, 0, m - 1, base);

    if (patternHash == textHash && pattern == text.substr(0, m)) {
        occurrences.push_back(0);
    }

    int power = 1;
    for (int i = 1; i <= m - 1; i++) {
        power = (power * base) % MOD;
    }

    for (int i = m; i < n; i++) {
        textHash = (((textHash - hashFunction(text[i - m]) * power) % MOD) * base) % MOD;
        textHash = (textHash + hashFunction(text[i])) % MOD;

        if (patternHash == textHash && pattern == text.substr(i - m + 1, m)) {
            occurrences.push_back(i - m + 1);
        }
    }
    return occurrences;
}

int main() {
    string text = "aaabbaaabbbaaabbbb";
    string pattern = "aaab";

    vector<int> occurrences = findPatternOccurrences(text, pattern);
    for (const auto& occurrence : occurrences) {
        cout << occurrence << " ";
    }
    return 0;
}
```

## 정리

Rolling Hash는 해시 비교를 `O(1)`로 끝내 평균 `O(n+m)`을 노리는 기법이다. 다만 해시 충돌이 잦으면 `==` 재확인 비용 때문에 최악 `O(n*m)`으로 퇴화하므로, **큰 소수 모듈러**와 **충돌이 적은 base** 선택이 성능을 좌우한다. 패턴이 짧고 입력이 클 때 특히 유리하다.
