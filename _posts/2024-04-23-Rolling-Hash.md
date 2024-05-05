---
layout: post
title: "Rolling Hash"
categories: c++
---

## Background

<!-- begin_excerpt -->
어떤 문자열에서 특정 문자열 패턴을 검색할 때 brutal force 방식을 사용하여 검색하면 입력 문자열 길이 n과 문자열 패턴 길이 m일 때 시간 복잡도는 O(n*m)이 된다.
<!-- end_excerpt -->

brutal force 방식을 코드로 짜면 문자열의 문자 하나씩 검색하려는 패턴 문자열의 문자와 일일이 비교하여 다르면 문자열의 다음 문자를 패턴 문자열과 처음부터 다시 검색하면서 진행한다.

이러한 방식은 입력 문자열과 문자열 패턴 길이가 길어질수록 O(n^2) 되어 지수적 시간 복잡도를 가져 효율성이 많이 떨어진다.

이런 문제를 효율적으로 해결하기 위한 방법으로 KMP 알고리즘, Boyer-Moore 알고리즘 등이 있는데 여기선 Rabin-Karp 알고리즘으로 알려진 간단한 Rolling Hash에 대해 살펴보려고 한다.

```c++
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// brutal force 방식으로 효율성이 많이 떨어진다.
vector<int> findPatternOccurrences(const string& text, const string& pattern) {
    vector<int> occurrences;
    int n = text.size();
    int m = pattern.size();

    for (int i = 0; i <= (n - m); i++) {
        int j = 0;
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

롤링 해쉬 알고리즘은 brutal force 방식의 시간 복잡도 O(n*m)를 입력 문자열과 패턴 문자열에 대한 해쉬 값 비교(for if hash compare)를 통해 최소 O(n+m) 시간 복잡도를 가지도록 개선한다.

입력 문자열과 패턴 문자열의 해쉬 값 생성은 문자열의 각 문자당 ascii code 값으로 할당(encoding)하고, ascii 문자는 같지만, 문자열의 자릿수가 다른 문자를 구분하기 위해 자릿수를 지숫값으로 hash 충돌이 덜 발생할 base (밑)을 정해 문자에 곱해 구분하여 만들어 준다.

```
입력 문자열 aaa에 대해

h(aaa) = h(a, a, a)
       = h(1, 1, 1)
       = 1 * 258^2 + 1 * 258^1 + 1 * 258^0
       = 66564 + 258 + 1
       = 66823
```
> <font size="2"> 
> 편이상 a의 ascii code가 1이라 치고 base는 ascii code 크기의 두 배로 넉넉히 258로 정했다.
> </font>

```
h(aaa) = h(a, a, a)
       = h(1, 1, 1)
       = (1 * 258^2 + 1 * 258^1 + 1 * 258^0) mod 1000000007
```
위에서 구한 hash 값은 문자열 길이가 조금만 길어지더라도 정수형 범위를 훌쩍 넘는데 modular 연산으로 값을 잘라준다.

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
> hash 충돌로 인해 hash 값은 같지만, 찾는 문자가 아니면 확인 비용만 증가한다. <br>
> c++ == 문자열 비교 연산자는 문자 하나씩 비교하다 다르면 그 즉시 false를 반환한다.
> </font>

위 코드는 hash 값 비교 시 O(1)의 시간 복잡도를 가지지만 string == 연산자를 통해 문자열 비교를 하여 O(m)의 시간 복잡도가 된다. 이러한 특성으로 인해 패턴 문자열 길이가 적을 때 rolling hash는 좀 더 유리한 경향이 있다.

```
258^99 mod 1000000007 = ?
```    
문자열 길이가 클 때 modular 연산 시에도 overflow가 발생할 수 있다. 이는 modular 연산 특성을 통해 아래와 같이 작은 수로 나눠서 계산 할 수 있다.

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

마지막으로 문자열의 인덱스를 하나씩 밀면서 (sliding window) hash 값을 구해 패턴 문자의 hash 값과 비교 후 같다면 찾는 문자열이 맞는지 확인하여 문자열의 패턴 위치를 돌려주면 된다.

```
    text: abcdefghijk

    h(abcd) = (a + b + c + d) mod 1000000007

    h(bcde) = (((h(abcd) - a * power mod 1000000007) mod 258) + e) mod 258
```

위 수식은 sliding window 계산식으로 복잡해 보이긴 하지만 한칸씩 밀면서 맨 앞의 값(a)을 빼고 뒤에 값(e)을 추가하는 단순한 수식임을 알 수 있다.

완성된 코드는 아래와 같다.

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
