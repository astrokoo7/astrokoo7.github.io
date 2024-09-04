#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <unordered_set>

using namespace std;

bool isPrime(int num) {
    if (num <= 1) return false; 

    for (int i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) {
            return false; 
        }
    }
    return true;
}

int solution(string numbers) {
    int answer = 0;
    unordered_set<int> nums;

    sort(numbers.begin(), numbers.end()); // 숫자들을 정렬하여 순열을 통해 중복된 조합을 피함

    do {
        for (int i = 1; i <= numbers.size(); ++i) {
            string temp = numbers.substr(0, i); // 현재 순열의 부분 문자열 생성
            int num_temp = stoi(temp); // 부분 문자열을 숫자로 변환
            if (isPrime(num_temp)) {
                nums.insert(num_temp); // 소수인 경우 집합에 추가
            }
        }
    } while (next_permutation(numbers.begin(), numbers.end())); // 다음 순열 생성

    return nums.size(); // 중복을 제거한 소수의 개수 반환
}


셀프 질문

어떻게 하면 문제를 작게 쪼갤 수 있을까?

어떻게 하면 문제를 단순화할 수 있을까?