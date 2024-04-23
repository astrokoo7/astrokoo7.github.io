---
layout: post
title: "Rolling Hash"
categories: c++
---

## Background

<!-- begin_excerpt -->
어떤 문자열에서 특정 문자열 패턴을 검색할 때 brutal force를 사용하여 검색하면 입력 문자열 길이 n과 문자열 패턴 길이 m일 때 시간 복잡도는 O(n*m)이 된다.
<!-- end_excerpt -->

코드로 짜보면 아래와 같고 문자열의 문자 하나씩 검색하려는 패턴 문자열과 비교하고 다르면 문자열의 다음 문자를 패턴 문자열과 처음부터 다시 검색하는 부분을 엿볼 수 있다.

이는 입력 문자열과 문자열 패턴 길이가 길어질수록 O(n^2) 되어 지수적 시간 복잡도를 가져 효율성이 많이 떨어지게 된다.

이러한 문제를 효율적으로 해결하기 위한 방법으로 KMP 알고리즘, Boyer-Moore 알고리즘 등이 있는데 여기선 Rabin-Karp 알고리즘으로 알려진 간단한 Rolling Hash에 대해 살펴보려고 한다.

```c++
#include <iostream>
#include <string>
#include <vector>

using namespace std;

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

롤링 해쉬 알고리즘은 간단히 말하면 입력 문자열에 대해 패턴 문자열을 이중 for loop로 검색했던 brutal force 방식을 for if 해쉬 값 비교로 최대 O(n+m) 시간 복잡도를 가지도록 만들어준다.

먼저 살펴볼 부분은 hash 값 생성 부분으로 문자열에 각 문자를 ascii code 값으로 할당하여 문자 간 서로 다른 hash 값을 가지도록 한다. 

또한 문자열 `aaa`의 문자를 그대로 ascii code로 hash 값을 만들어 사용한다면 같은 문자 `a`끼리 hash 충돌이 발생하여 이를 방지하기 위해 각 문자열 자릿수를 지숫값으로 hash 충돌이 덜 발생할 base (밑)을 정해 곱하여 더해준다.

```
h(aaa) = h(a, a, a)
       = h(1, 1, 1)
       = 1 * 258^2 + 1 * 258^1 + 1 * 258^0
       = 66564 + 258 + 1
       = 66823
```
> <font size="2"> 
> 편이상 a의 ascii code가 1이라 치고 base는 ascii code의 두 배로 넉넉히 258로 정했다.
> </font>

위에서 구한 hash 값은 문자열 길이가 조금만 길어지더라도 정수형 범위를 훌쩍 넘는다.


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
