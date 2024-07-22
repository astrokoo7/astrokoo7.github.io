#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int solution(vector<int> A, vector<int> B) {
    int answer = -1;

    sort(begin(A), end(A));
    sort(begin(B), end(B));

    int cur = -1;

    for (int i = 0; i < A.size(); i++) {
        auto a = A[i];
        auto b = B[i];

        cur = a - b;



    }

    return answer;
}

int main()
{
    std::cout << "Hello World!\n";
}


https://velog.io/@e7838752/programmers-numberGame