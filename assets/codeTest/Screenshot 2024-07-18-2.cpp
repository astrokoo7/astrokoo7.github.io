#include <iostream>
#include <vector>
#include <algorithm>

    int solution(std::vector<int>& money) {
        if (money.size() <= 3) {
            return *std::max_element(money.begin(), money.end());
        }

        std::vector<int> dp1(money.begin(), money.end() - 1);
        std::vector<int> dp2(money.begin() + 1, money.end());

        for (int i = 2; i < money.size() - 1; i++) {
            dp1[i] += std::max(dp1[i - 2], i >= 3 ? dp1[i - 3] : 0);
            dp2[i] += std::max(dp2[i - 2], i >= 3 ? dp2[i - 3] : 0);
        }

        return std::max({
            dp1[dp1.size() - 1],
            dp1[dp1.size() - 2],
            dp2[dp2.size() - 1],
            dp2[dp2.size() - 2]
            });
    }


int main()
{
    std::vector<int> b = { 1, 2, 3, 1, 5, 7 };
    auto a = solution(b);

    std::cout << "Hello World!\n";
}