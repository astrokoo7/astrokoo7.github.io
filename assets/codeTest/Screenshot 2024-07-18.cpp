#include <vector>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

long long calculate(long long a, long long b, char op) {
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    return 0; // This should never happen
}

int solution(vector<string> arr) {
    int n = arr.size();
    vector<long long> numbers;
    vector<char> ops;
    
    // Parse the input
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            numbers.push_back(stoll(arr[i]));
        } else {
            ops.push_back(arr[i][0]);
        }
    }
    
    n = numbers.size();
    vector<vector<long long>> dp_max(n, vector<long long>(n, LLONG_MIN));
    vector<vector<long long>> dp_min(n, vector<long long>(n, LLONG_MAX));
    
    // Initialize dp with single numbers
    for (int i = 0; i < n; i++) {
        dp_max[i][i] = dp_min[i][i] = numbers[i];
    }
    
    // Fill dp tables
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i < n - len + 1; i++) {
            int j = i + len - 1;
            for (int k = i; k < j; k++) {
                long long left_max = dp_max[i][k];
                long long left_min = dp_min[i][k];
                long long right_max = dp_max[k+1][j];
                long long right_min = dp_min[k+1][j];
                char op = ops[k];
                
                vector<long long> results = {
                    calculate(left_max, right_max, op),
                    calculate(left_max, right_min, op),
                    calculate(left_min, right_max, op),
                    calculate(left_min, right_min, op)
                };
                
                dp_max[i][j] = max(dp_max[i][j], *max_element(results.begin(), results.end()));
                dp_min[i][j] = min(dp_min[i][j], *min_element(results.begin(), results.end()));
            }
        }
    }
    
    return (int)dp_max[0][n-1];
}

/*
INF = 987654321
n = 숫자 갯수임.
MIN_DP = [[INF for _ in range(n)] for _ in range(n)]
MAX_DP = [[-INF for _ in range(n)] for _ in range(n)]

for step in range(len(DP)): #i와 j의 간격.

    for i in range(len(DP)-step): #i부터 j까지의 연산을 수행함.

        j = i + step

        if step == 0:
            DP[i][i] = 해당되는 숫자
        else:
            for k in range(i, j): #i 부터 j까지 돌면서, 괄호를 하나는 늘리고, 하나는 줄여서 각각의 범위 연산을 수행함.
                if k번째의 연산자 == '+': #k번째에 해당되는 연산이 + 일 경우:
                    MAX_DP[i][j] = max(MAX_DP[i][j], MAX_DP[i][k] + MAX_DP[k+1][j]) # + 일 경우 최댓값은 최댓값 + 최댓값임.
                    MIN_DP[i][j] = min(MIN_DP[i][j], MIN_DP[i][k] + MIN_DP[k+1][j]) # + 일 경우 최솟값은 최솟값 + 최솟값임.
               else: #k번째에 해당되는 연산이 - 일 경우.
                    MAX_DP[i][j] = max(MAX_DP[i][j], MAX_DP[i][k] - MIN_DP[k+1][j]) # - 일 경우 최댓값은 최댓값 - 최솟값임.
                    MIN_DP[i][j] = min(MIN_DP[i][j], MIN_DP[i][k] - MAX_DP[k+1][j])# - 일 경우 최솟값은 최솟값 - 최댓값임.

return MAX_DP[0][n-1] 
*/

https://school.programmers.co.kr/questions/35224