//https://school.programmers.co.kr/learn/courses/30/lessons/152995
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

int solution(vector<vector<int>> scores) {
    int answer = 1;
    int n = scores.size();
    int prevN = -1;
    auto x = scores[0][0] + scores[0][1];

    sort(scores.begin()+1, scores.end(), [](const vector<int>& a, const vector<int>& b) {
        return a[0] > b[0] || (a[0] == b[0] && a[1] < b[1]);
        });

    for (int i = 1; i < n; i++) {
        auto y = scores[i][0] + scores[i][1];

        if (y > x && scores[i][1] >= prevN) {
            prevN = scores[i][1];

            answer++;
            if (scores[i][0] > scores[0][0] &&
                scores[i][1] > scores[0][1]) {
                return -1;
            }
        }
    }
    return answer;
}

int main()
{
    //완호보다 점수가 높은 사람중에 인센티브가 -1 인 사람이 있을 수 있습니다.
    //[[4,1],[2,4],[3,5]]
    //이 경우 완호 다음 직원은 완호보다 총 점은 높지만 마지막 직원 보다 점수가 둘다 낮기 때문에 인센티브를 받을 수 없고, 완호는 2등이 됩니다.

    auto a = solution({ { 4, 1 }, { 2, 4 }, { 3, 5 } });

    std::cout << "Hello World!\n";
}
