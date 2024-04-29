
#include <iostream>
#include <sstream>
#include <string>


    stringstream iss;
    string buffer;
    vector<string> result;

    while (getline(iss, buffer, ' ')) {
      result.push_back(buffer);
    }


가장 많이 받은 선물

    빠진 경우의 수

    map + pair 일땐 
    
    A -> B 인데 B -> A 에게 보낸게 없을땐 catch 가능

    A -> C 이고 B -> D 이면 A와 B 는 서로 주고 받은 기록이 아예 없음

    이때도 A와 B의 지수 비교가 필요함

    한데 기록 중심이면 아예 기록이 없어 counting 자체를 안함.



이것뿐만 아니라 A 기준으로 비교시 A 기준으로 만족 할때만 +1 해주고

반대의 경우엔 그 기준 데이터가 위처럼 똑같이 체크 할테니 A에서 하는 것 

자체가 말이 안됬음.



    vector<vector<int>> points;
    map<pair<string, string>, int> points;
    map<string, int> totalPoints;
    vector<int> totalPoints;

자료 구조 선택부터가 에러였음

두 사람이 선물을 주고받은 기록이 하나도 없거나 주고받은 수가 같다면


map<string, int> nameYearning;
vs
unordered_map<string, int> nameYearning; 

    year = stoi(today.substr(0, 4)); month = stoi(today.substr(5, 2)); day = stoi(today.substr(8, 2));
    total = year * 12 * 28 + (month - 1) * 28 + day;
    
    for(int i = 0; i < terms.size(); i++){
        stringstream ss(terms[i]);
        char alpha; int month;
        ss >> alpha >> month;
        t_map[alpha] = month;
    }
    
    
	
키패드 누르기



pair 오류

map<pair<int, int>, int> keyPad;

keyPad.emplace({ i, j }, 10); X

keyPad.emplace(pair<int, int>{ i, j }, 10);
keyPad.emplace(make_pair(i, j), 10);


generic

template<typename Container>
void printContainer(const Container& container) {
    for (const auto& element : container) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}



두 쌍에 대해

각각 찾고 

없다면 넣고 

set<int> a

for (all set)
    a.find(first)
    a.find(second)

    not at all
        new set and insert first, second

    else
        



k-d tree 

https://velog.io/@rondido/coding-Testjavascript-%EA%B2%BD%EC%B0%B0%EA%B3%BC-%EB%8F%84%EB%91%91%EA%B7%B8%EB%A6%AC%EB%94%94


https://bowbowbow.tistory.com/25



window 
ascii 로 ecoding 

adcd == dcba same hash value so error
permutation 조합

자리수 만큼 * 10 해서 위 문제 해결

문자열이 크면 overflow 될 수 있음

mod 로 나눈다. 적당히 hash collision 이 안날 소수로 

10^99 mod 113 같은 건 어떻게?


    int power = 1;
    for (int i = 1; i <= 99; i++) {
        power = (power * 10) % 103;
    }

(a * b) mod c = ((a mod c) * (b mod c)) mod c

https://m.blog.naver.com/bujya_road/222759739376


https://goldenrabbit.co.kr/2023/12/01/%EC%BD%94%EB%94%A9-%ED%85%8C%EC%8A%A4%ED%8A%B8-%ED%95%A9%EA%B2%A9%EC%9E%90-%EB%90%98%EA%B8%B0-%ED%95%B4%EC%8B%9C-2-%ED%95%B4%EC%8B%9C-%ED%95%A8%EC%88%98%EC%99%80-%EC%B6%A9%EB%8F%8C-%EC%B2%98/








#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

// Trie 노드 구조체
struct TrieNode {
    TrieNode* children[26] = { nullptr };
    bool isEndOfWord = false;
};

// Trie 클래스
class Trie {
public:
    Trie() : root(new TrieNode()) {}
    ~Trie() { deleteTree(root); }

    void insert(const std::string& word) {
        TrieNode* node = root;
        for (char c : word) {
            int index = c - 'a';
            if (!node->children[index]) {
                node->children[index] = new TrieNode();
            }
            node = node->children[index];
        }
        node->isEndOfWord = true;
    }

    bool search(const std::string& word) {
        TrieNode* node = searchNode(word);
        return node && node->isEndOfWord;
    }

    bool startsWith(const std::string& prefix) {
        TrieNode* node = searchNode(prefix);
        return node != nullptr;
    }

private:
    TrieNode* root;

    TrieNode* searchNode(const std::string& word) {
        TrieNode* node = root;
        for (char c : word) {
            int index = c - 'a';
            if (!node->children[index]) {
                return nullptr;
            }
            node = node->children[index];
        }
        return node;
    }

    void deleteTree(TrieNode* node) {
        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                deleteTree(node->children[i]);
            }
        }
        delete node;
    }
};

std::vector<std::string> soln(const std::vector<std::string>& words) {
    std::vector<std::string> reports;
    std::unordered_set<std::string> badWords;
    Trie trie;

    for (const std::string& word : words) {
        if (badWords.count(word) || trie.search(word)) {
            reports.push_back(word);
        }
        else {
            badWords.insert(word);
            trie.insert(word);
        }
    }

    std::cout << "Total bad words: " << badWords.size() << std::endl;
    for (const std::string& badWord : badWords) {
        std::cout << badWord << std::endl;
    }

    return reports;
}

int main() {
    std::vector<std::string> words = { "abcd", "abcde", "sdkfkkoxcc", "dkfuds", "kdgugs", "dkfdu", "dsf", "dkjfgusdgg", "dkdkfdufd", "abcdef", "kkoxcc" };

    std::vector<std::string> reports = soln(words);
    std::cout << "Reported words:" << std::endl;
    for (const std::string& report : reports) {
        std::cout << report << std::endl;
    }

    return 0;
}

https://blog.naver.com/babobigi

https://www.youtube.com/watch?v=cjWnW0hdF1Y

https://www.youtube.com/watch?v=2pcNxd2t6uc

https://www.youtube.com/watch?v=HXU17rokY-o

재귀 함수는 왜 중복 실행이 많을까?

그래서 메모니제이션을 한다.

int fibonacci(int n) {
    vector<int> fib(n + 1);
    fib[0] = 0;
    fib[1] = 1;

    for (int i = 2; i <= n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    return fib[n];
}

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    int a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }

    return b;
}

// 재귀 버전
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 메모이제이션 버전
vector<int> memo(51, -1); // 메모이제이션 배열 초기화

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    
    // 메모이제이션 체크
    if (memo[n] != -1) {
        return memo[n];
    }
    
    // 메모이제이션
    memo[n] = fibonacci(n - 1) + fibonacci(n - 2);
    
    return memo[n];
}



	이전과 현재 차이가 k 보다 같거나 작은지 확인

	결과1 = 현재와 다음 인덱스 + 1

	결과2 = 이전과 다음 인덱스

