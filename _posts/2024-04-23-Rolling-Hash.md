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
