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
                long long left_max = dp_max[i][k], left_min = dp_min[i][k];
                long long right_max = dp_max[k+1][j], right_min = dp_min[k+1][j];
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