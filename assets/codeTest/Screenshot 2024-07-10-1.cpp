//https://school.programmers.co.kr/learn/courses/30/lessons/152995

#include <string>
#include <vector>

using namespace std;

int solution(vector<vector<int>> scores) {
    int answer = 1;
    int n = scores.size();
    auto x = scores[0][0] + scores[0][1];
    for (int i=1; i<n; i++) {
        auto y = scores[i][0] + scores[i][1];
        if (y > x) {
            answer++;
            if (scores[i][0] > scores[0][0] && 
                scores[i][1] > scores[0][1]) {
                return -1;
            }
        }
    }
    return answer;
}