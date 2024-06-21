---
layout: post
title: "Rolling Hash"
categories: c++
---

<!-- begin_excerpt -->
어떤 문자열에서 특정 문자열 패턴을 검색할 때 brutal force 방식을 사용하여 검색하면, <br>
입력 문자열 길이 n과 문자열 패턴 길이 m일 때 시간 복잡도는 O(n*m)이 된다.
<!-- end_excerpt -->

보통 입력 문자열과 찾으려는 패턴이 짧은 경우 brutal force 방식을 사용하지만, <br>
리눅스 grep 명령어처럼 아주 큰 입력 문자열에서 긴 문자열 패턴을 찾으려 할 땐 적합하지 않다.

이러한 상황에선 KMP, Boyer-Moore 같은 알고리즘을 주로 사용하는데,<br>
여기선 Rabin-Karp 으로 알려진 간단한 Rolling Hash에 대해 살펴보려고 한다.

```c++
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// brutal force 방식으로 긴 문자열에서 검색할 땐 적합하지 않다.
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

롤링 해쉬 알고리즘은 입력 문자열과 패턴 문자열에 대한 해쉬 값 비교(for if hash compare)를 통해 <br>
최대 O(n+m) 시간 복잡도를 가지게 만들어준다.

- [Generate Hash](#generate-hash)
- [Sliding Hash Window](#sliding-hash-window)

### Generate Hash

해쉬 값 생성은 문자열의 각 문자당 ascii code 값으로 할당(encoding)하고, ascii 문자는 같지만, <br>
문자열의 위치가 다른 문자를 구분하기 위해 자릿수를 지숫값으로 hash 충돌이 덜 발생할 base (밑)을 정해 문자에 곱하여 만들어 준다.

```
입력 문자열 aaa에 대한 hash를 생성해 보자

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
위에서 구한 hash 값은 문자열 길이가 조금만 길어지더라도 정수형 범위를 훌쩍 넘는데 <br>
modular 연산으로 값을 잘라준다.

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
> 애초에 hash 충돌이 덜 발생하게 만들어야 하는 이유다. <br>
> 참고로, c++ == 문자열 비교 연산자는 brutal force 방식으로 문자열을 비교한다.
> </font>

위 코드는 hash 값 비교 시 O(1)의 시간 복잡도를 가지지만 string == 연산자를 통해 문자열 비교를 하여 O(m)의 시간 복잡도가 된다. <br>
이러한 특성으로 인해 패턴 문자열 길이가 적을 때 rolling hash는 좀 더 유리한 경향이 있다.

```
258^99 mod 1000000007 = ?
```    
문자열 길이가 길면 modular 연산 시에도 overflow가 발생할 수 있다. <br>
이는 modular 연산 특성을 통해 아래와 같이 작은 수로 나눠서 계산 할 수 있다.

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

최초 패턴 문자열의 길이만큼 문자열에서 hash 값을 생성하고, 패턴 문자열 hash 와 비교 후 다르면 <br>
문자열의 인덱스를 하나씩 밀면서 hash 값을 갱신 후 다시 비교하면서 일치하는 문자열 패턴을 찾을 수 있다.

```
    text: abcdefghijk

    h(abcd) = (a + b + c + d) mod 1000000007

    h(bcde) = (((h(abcd) - a * power mod 1000000007) mod 258) + e) mod 258
```

위 수식은 Sliding Hash Window 계산식으로 복잡해 보이긴 하지만 한 칸씩 문자열의 인덱스를 밀면서 맨 앞의 값(a)을 빼고 뒤에 값(e)을 추가하여 해시값을 갱신할 수 있으며, <br>
갱신된 해쉬 값과 패턴 문자열의 해쉬 값 비교를 통해 문자열에서 패턴 문자열을 찾을 수 있다.

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
