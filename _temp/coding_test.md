플로이드-워셜 알고리즘


1. 단순구현문제 골드까지 풀기(단계별)
한 문제를 시간제한없이 물고 늘어지지 말기
 - 시간제한을 정하여 해당시간 내에 답이 안나오면 과감히 답지를 보기(30분~1시간)
 - 이때, 가볍게 보고 넘기는것이 아니라 코드 한줄한줄을 완벽에 가깝게 이해하며 누군가에게 코드의 의도를 설명할 수 있을정도로 이해해야된다.

2. 복습문제 queue 관리
 - 답지를 보고 해결한 문제들을 모아놓고 특정 날이나 시간을 정해 그것들만 다시보자

3. 유형별로 풀기
 - 단계별로 풀기에 더해 알고리즘 유형별로 도장깨기 (단순구현 -> 완전탐색 -> 핵심 알고리즘 -> 복잡하고 어려운 알고리즘)

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
