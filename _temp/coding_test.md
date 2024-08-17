#include <iostream>
#include <vector>

void printCombination(const std::vector<int>& combination) {
    for (int num : combination) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

void generateCombinations(const std::vector<int>& elements, int start, int k, std::vector<int>& current_combination) {
    if (k == 0) {
        printCombination(current_combination);
        return;
    }

    for (int i = start; i <= elements.size() - k; ++i) {
        current_combination.push_back(elements[i]);
        generateCombinations(elements, i + 1, k - 1, current_combination);
        current_combination.pop_back();
    }
}

int main() {
    std::vector<int> elements = {1, 2, 3};
    int k = 2;
    std::vector<int> current_combination;

    generateCombinations(elements, 0, k, current_combination);
    return 0;
}


#include <iostream>
#include <vector>
#include <algorithm>

// 조합 출력 함수
void printCombination(const std::vector<int>& combination) {
    for (int num : combination) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

// 재귀적으로 순열을 생성하여 조합으로 변환
void generatePermutations(std::vector<int>& elements, int start, int k, std::vector<int>& current_combination) {
    if (current_combination.size() == k) {
        printCombination(current_combination);
        return;
    }

    for (int i = start; i < elements.size(); ++i) {
        current_combination.push_back(elements[i]);
        std::swap(elements[start], elements[i]);
        generatePermutations(elements, start + 1, k, current_combination);
        std::swap(elements[start], elements[i]);
        current_combination.pop_back();
    }
}

// 조합 생성 함수
void generateCombinations(std::vector<int>& elements, int k) {
    std::vector<int> current_combination;
    generatePermutations(elements, 0, k, current_combination);
}

int main() {
    std::vector<int> elements = {1, 2, 3};
    int k = 2;

    generateCombinations(elements, k);

    return 0;
}
