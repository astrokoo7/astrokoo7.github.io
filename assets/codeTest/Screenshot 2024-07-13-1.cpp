#include <vector>
#include <unordered_map>

class Solution {
public:
    int solution(std::vector<int>& a) {
        int answer = 0;
        int N = a.size();

        // 스타 수열의 길이를 계산하는 함수
        auto lengthOfStarSequence = [&](int key, const std::vector<int>& idxs) {
            int count = 0;
            std::vector<bool> used(N, true);

            for (int i : idxs) {
                if (!used[i]) continue;

                // 1,2,1,3,4,1
                // 0,2,5
                // 1,2
                // 2,1
                // 1,3

                // 왼쪽 원소와 조합
                if (i > 0 && a[i] != a[i - 1] && used[i - 1]) {
                    used[i] = false;
                    count += 2;
                }
                // 오른쪽 원소와 조합
                else if (i < (N - 1) && a[i] != a[i + 1] && used[i + 1]) {
                    used[i + 1] = false;
                    count += 2;
                }
            }

            return count;
            };

        // { 원소 : [원소 인덱스...] }
        std::unordered_map<int, std::vector<int>> dictionary;
        for (int i = 0; i < N; ++i) {
            dictionary[a[i]].push_back(i);
        }

        for (const auto& pair : dictionary) {
            // 가지치기
            if (pair.second.size() * 2 < answer) continue;
            answer = std::max(answer, lengthOfStarSequence(pair.first, pair.second));
        }

        return answer;
    }
};
