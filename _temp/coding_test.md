
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

	현재와 다음 차이가 k 보다 같거나 작은지 확인

	만족하면 다음과 다음의 다음으로 진행
		결과1 += 1

	결과2 = 현재와 다음의 다음 인덱스 (바로 다음은 건너띈 것의 총합)

	max(결과1, 결과2)


    i, i+1
    i+1, i+2

    i, i+2

    1현재가 끝인가? 종료


    1현재와 2현재가 같은가?
        1 + 재귀

    1현재와 2 다음이 같은가?
        재귀




class Solution {
public:
    int foo(string text1, string text2, int idx1, int idx2) {

        if (text1.size() == idx1) return 0;

        int result = 0;

        if (text1[idx1] == text2[idx2]) {

            result = 1 + foo(text1, text2, idx1+1, idx2+1);
        } else {
            result = foo(text1, text2, idx1+1, idx2);
        }



        return result;
}

    int longestCommonSubsequence(string text1, string text2) {

        return foo(text1, text2, 0, 0);
    }
};

https://leetcode.com/problems/longest-common-subsequence/


코딩이 문제가 아니고 경우의 수를 찾고 

그걸 논리적으로 기술하면 된다.


// 0430.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

using namespace std;

int fistIdx = -1;
int nextIdx = 0;


int foo(string text1, string text2, int idx1, int idx2) {

    if (text1.size() <= idx1 || text2.size() <= idx2) {
        return 0;
    }

    int result = 0;
    int nextResult = 0;

    if (text1[idx1] == text2[idx2]) {

        if (fistIdx == -1) {
            fistIdx = idx1;
        }
        nextIdx = idx2 + 1;

        result = 1 + foo(text1, text2, idx1+1, idx2+1);

        if (fistIdx == idx1) {
            fistIdx = -1;
            nextIdx = 0;
            nextResult = foo(text1, text2, idx1+1, 0);
        }
        else {
            int a = 0;
            a;
        }

        result = max(result, nextResult);
    }
    else {
        if (text2.size() == idx2 + 1) {
            result = foo(text1, text2, idx1 + 1, nextIdx);
        }
        else {
            result = foo(text1, text2, idx1, idx2 + 1);
        }
    }

    return result;
}


int main()
{
    // p
    // qr

    // p 부터 시작해 최대 길이 문자 길이 값
    // q 부터 시작해 최대 길이 문자 길이 값

    // 을 동등히 비교해서 제일 큰 값을 찾는다.


    // 저장이 필요해서 
    // 저장을 했더니 제일 처음 저장한 값만 가져오더라

    //int d = foo("abc", "ab", 0, 0);
    //if (d == 2) {
    //    cout << d;
    //    cout << endl;
    //}


    // 근데 이 경우에도 뒤로 못하게 하면 
    // 가장 긴 거리를 찾지 못하게 됨

    // pr 로 잘못 된 로직이지만 결과는 2로 맞음
    // 원래는 qr 로 2가 되어야 함
    // 이것부터 해결 필요
    // 그러니깐 어쩌다 맞춘것일뿐
    // 

    // 그래서 p 에서 선택된 문자 기준 앞에 문자를 선택 못하게 해봤는데
    // q 관점에서 더 긴 문자열을 찾을 수 있음에도 찾기 못하게 됨

    // p가 시작점이 되서 모든 해당하는 수를 찾는 것과
    // q가 시작점이 되서 모든 해당하는 수를 찾는 것이 분리 되어야 함
    // 

    // 시작점은 제일 처음 매칭 포인트를 찾았을 때로 가정해보자
    // 이건 p 다음은 나오는 모든 매칭 포인트는 선택 안하게 됨?

    int a = foo("oxcpqrsvwf", "shmtulqrypy", 0, 0);
    if (a == 2) {
        cout << a;
        cout << endl;
    }

    //// ace
    //// ce
    //// e

    int b = foo("abcde", "ace", 0, 0);
    if (b == 3) {
        cout << b;
        cout << endl;
    }
    else {
        cout << b;
        cout << endl;
    }

    //// up
    //// p

    // 이 경우만 보면 성공한 마지막을 저장하고 뒤로는 검색 못하게 하는게
    // 맞아 보임
    int c = foo("ezupkr", "ubmrapg", 0, 0);
    if (c == 2) {
        cout << c;
        cout << endl;
    }
    else {
        cout << c;
        cout << endl;
    }


    //
    //


    return 0;
}



// 0430.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int foo(string text1, string text2, int idx1, int idx2, int nextIdx) 
{
    if (text1.size() <= idx1 || text2.size() <= idx2) {
        return 0;
    }

    int result = 0;

    if (text1[idx1] == text2[idx2]) {
        int prevR = 0;
        for (int i = 1; i < text1.size(); i++) {
            int tmp = foo(text1, text2, idx1 + i, idx2 + 1, idx2 + 1);
            prevR = max(prevR, tmp);
        }
        result = 1 + prevR;
    }
    else {
        if (text2.size() == idx2 + 1) {
            result = foo(text1, text2, idx1 + 1, nextIdx, nextIdx);
        }
        else {
            result = foo(text1, text2, idx1, idx2 + 1, nextIdx);
        }
    }

    return result;
}

int bar(string text1, string text2)
{
    int lastResult = 0;
    for (int i = 0; i < text1.size(); i++) {
        int result = foo(text1, text2, i, 0, 0);
        lastResult = max(lastResult, result);
    }
    return lastResult;
}

int main()
{


    // p
    // qr

    // p 부터 시작해 최대 길이 문자 길이 값
    // q 부터 시작해 최대 길이 문자 길이 값

    // 을 동등히 비교해서 제일 큰 값을 찾는다.


    // 저장이 필요해서 
    // 저장을 했더니 제일 처음 저장한 값만 가져오더라

    //int d = foo("abc", "ab", 0, 0);
    //if (d == 2) {
    //    cout << d;
    //    cout << endl;
    //}


    // 근데 이 경우에도 뒤로 못하게 하면 
    // 가장 긴 거리를 찾지 못하게 됨

    // pr 로 잘못 된 로직이지만 결과는 2로 맞음
    // 원래는 qr 로 2가 되어야 함
    // 이것부터 해결 필요
    // 그러니깐 어쩌다 맞춘것일뿐
    // 

    // 그래서 p 에서 선택된 문자 기준 앞에 문자를 선택 못하게 해봤는데
    // q 관점에서 더 긴 문자열을 찾을 수 있음에도 찾기 못하게 됨

    // p가 시작점이 되서 모든 해당하는 수를 찾는 것과
    // q가 시작점이 되서 모든 해당하는 수를 찾는 것이 분리 되어야 함
    // 

    // 시작점은 제일 처음 매칭 포인트를 찾았을 때로 가정해보자
    // 이건 p 다음은 나오는 모든 매칭 포인트는 선택 안하게 됨?

    //int a = bar("oxcpqrsvwf", "shmtulqrypy");
    //if (a == 2) {
    //    cout << a;
    //    cout << endl;
    //}


    //// ace
    //// ce
    //// e

    //int b = foo("abcde", "ace", 0, 0, 0);
    //if (b == 3) {
    //    cout << b;
    //    cout << endl;
    //}
    //else {
    //    cout << b;
    //    cout << endl;
    //}

    //// up
    //// p

    // 이 경우만 보면 성공한 마지막을 저장하고 뒤로는 검색 못하게 하는게
    // 맞아 보임
    //int c = foo("ezupkr", "ubmrapg", 0, 0, 0);
    //if (c == 2) {
    //    cout << c;
    //    cout << endl;
    //}
    //else {
    //    cout << c;
    //    cout << endl;
    //}
    
    //mhz iwb
    //mhz q
    //     

    string text1 = "ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    string text2 = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

    dp.resize(text1.size());
    for (int i = 0; i < text1.size(); i++) {
        dp[i].resize(text2.size(), -1);
    }

    int d = bar(text1, text2);
    if (d == 6) {
        cout << d;
        cout << endl;
    }
    else {
        cout << d;
        cout << endl;
    }


    //
    //


    return 0;
}

public class Solution {
    public int LongestCommonSubsequence(string text1, string text2) {
        int m = text1.Length;
        int n = text2.Length;

        int[] dp = new int[n + 1];

        for (int i = 1; i <= m; i++) {
            int prev = 0;  // Initialize the value for the previous diagonal element

            for (int j = 1; j <= n; j++) {
                int temp = dp[j];  // Save the current value in dp[j] for the next iteration
                if (text1[i - 1] == text2[j - 1]) {
                    dp[j] = prev + 1;
                } else {
                    dp[j] = Math.Max(dp[j], dp[j - 1]);
                }
                prev = temp;  // Update prev for the next iteration
            }
        }

        return dp[n];
    }
}

int dp[1001][1001];

int find(int p1, int p2, string &s1, string &s2)
{

    if(p1 == s1.length() || p2 == s2.length())
        return 0;

    if(dp[p1][p2] != -1)
        return dp[p1][p2];

    if(s1[p1] == s2[p2])
        return dp[p1][p2] = ( 1 + find(p1+1, p2+1, s1, s2) );
 
    else 
        return dp[p1][p2] = max(find(p1+1, p2, s1, s2) , find(p1, p2+1, s1, s2));

    return 0;
}


class Solution {
public:
    int longestCommonSubsequence(string t1, string t2) {

        memset(dp, -1, sizeof(dp));
        return find(0, 0, t1, t2);
    }
};


int foo(string s, int p1, int p2)
{
    std::cout << p1 << "," << p2 << endl;

    if (p2 < p1 || p1 < 0 || p2 < 0)
        return 0;

    if (p1 == p2)
        return 1;

    int result = 0;
    if (s[p1] == s[p2]) {
        return 2 + foo(s, p1 + 1, p2 - 1);
    }
    else {
        return max(foo(s, p1, p2 - 1), foo(s, p1 + 1, p2));
    }
}

class Solution {
public:

    ListNode* removeNodes(ListNode* head) {
        // if (head == nullptr) {
        //     return nullptr;
        // }

        stack<ListNode*> stack;
        stack.push(head);

        ListNode* node = head->next;
        while (node != nullptr) {
            stack.push(node);
            node = node->next;
        }

        ListNode* nxtGreater = nullptr;
        
        while (!stack.empty()) {
            node = stack.top();
            stack.pop();
            node->next = nxtGreater;

            if (nxtGreater == nullptr || node->val >= nxtGreater->val) {
                nxtGreater = node;
            } else {
                nxtGreater = node->next;
            }
        }

        return nxtGreater;

        // // push
        // ListNode* nxtGreater = removeNodes(head->next);


        // // pop
        // head->next = nxtGreater;
        // if (nxtGreater == nullptr || head->val >= nxtGreater->val) {
        //     return head;
        // }
        // return nxtGreater;
    }
};



#include <iostream>
#include <stack>

using namespace std;

int Fi(int num)
{
    cout << num << endl;

    if (num == 1)
        return 1;
    else if (num == 2)
        return 1;
    else
        return Fi(num - 1) + Fi(num - 2);
}

// 0, 1, 1, 2, 3, 5, 8, 13, 21

int foo(int num)
{
    stack<int> stack_;
    stack_.push(num);

    int result = 0;

    while (!stack_.empty()) {

        auto num2 = stack_.top();
        stack_.pop();

        //cout << num2 << endl;

        if (num2 == 1) {
            result += 1;
        }
        else if (num2 == 2) {
            result += 1;
        }
        else {
            stack_.push(num2 - 2);
            stack_.push(num2 - 1);
        }
    }

    return result;
}


int main()
{
    Fi(10);
    foo(10);

    return 0;
}

// 0512.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> result;

//vector<string> generateParenthesis(int n) {
//
//    for (int i = n; -1 < i; i--) {
//        string s;
//        foo(i, s);
//    }
//
//    return result;
//}


string foo(int n, string& s)
{
    if (n == 0) {
        return s;
    }

    while (!(n < 0)) {

        // ((()))
        s.push_back('(');
            s.push_back('(');
                s.push_back('(');

        // (()())
        s.push_back('(');
            s.push_back('(');
            s.push_back('(');

        // (())()
        s.push_back('(');
            s.push_back('(');
        s.push_back('(');

        // ()(())
        s.push_back('(');
        s.push_back('(');
            s.push_back('(');

        s.push_back('(');
        s.push_back('(');
        s.push_back('(');

        // (()()()())
        // ((()())())
        // ((()()()))
    }
}

void bar(int n, string& s)
{
    s.push_back('(');

    if (0 < n) {

        bar(n-1, s);


        bar(0, s);
        bar(0, s);
        bar(0, s);
    }

    s.push_back(')');
}

int main()
{
    {
        string s;
        bar(1, s);
        //bar(0, s);
        cout << s << endl;
    }

    //{
    //    string s;
    //    bar(1, s);
    //    bar(0, s);
    //    cout << s << endl;
    //}

    //{
    //    string s;
    //    bar(0, s);
    //    bar(1, s);
    //    cout << s << endl;
    //}

    // call 3
    //bar(0, s);

    //auto r = generateParenthesis(3);

    return 1;

}





https://kldp.org/node/39437

https://github.com/microsoft/WSL/issues/4926

https://www.algodale.com/guides/

https://dalgona.dev/posts/2018-02-12-memoization-in-elixir-part-1.html

https://learn.microsoft.com/en-us/powershell/module/netnat/remove-netnat?view=windowsserver2022-ps



void generateParenthesesHelper(std::vector<std::string>& result, std::string current, int open, int close, int max) {
    if (current.length() == max * 2) {
        result.push_back(current);
        return;
    }

    //  (           
    //  ((                          
    //  (((         )
    //  (((         ))
    //  (((         )))

    //  ((          )
    //  ((          ))
    //  ((          )))

    if (open < max)
        generateParenthesesHelper(result, current + "(", open + 1, close, max);
    if (close < open)
        generateParenthesesHelper(result, current + ")", open, close + 1, max);
}

class Solution {
public:

        map<char, string> keyPad = {
            {'2', "abc"},
            {'3', "def"},
            {'4', "ghi"},
            {'5', "jkl"},
            {'6', "mno"},
            {'7', "pqrs"},
            {'8', "tuv"},
            {'9', "wxyz"},
        };

    void foo(vector<string>& result, string& digits, int idx) 
    {
        if (idx == digits.size()) 
            return;
        auto c = digits[idx];
        auto str = keyPad[c];

        for (auto c2 : str) {

            foo(result, digits, idx + 1);

        }



    }


    vector<string> letterCombinations(string digits) {



        vector<string> ret;

        return ret;
    }
};


using namespace std;

string longestPalindrome(string s) {

    stack<string> stack_;

    for (auto c : s) {
        if (stack_.empty()) {
            stack_.push(string(1, c));
        }
        else {
            auto str = stack_.top();
            stack_.push(str + c);
        }
    }
    
    // 붙이면서 동등이 됬는지 검사

    for (int i = 0; i < stack_.size(); i++) {

        auto c = stack_[i];
    }


    return "";
}

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// 데이터 포인트 구조체
struct Point {
    int dim;
    vector<int> val;
    Point(vector<int> v) : val(v) {
        dim = v.size();
    }
};

// KD 트리 노드 구조체
struct Node {
    Point p;
    int split;
    Node* left;
    Node* right;
    Node(Point p, int split) : p(p), split(split), left(nullptr), right(nullptr) {}
};

// KD 트리 생성 및 검색 함수
Node* makeTree(vector<Point>& points, int depth) {
    if (points.empty())
        return nullptr;

    int split = depth % points[0].dim;
    sort(points.begin(), points.end(), [split](Point a, Point b) {
        return a.val[split] < b.val[split];
    });

    int mid = points.size() / 2;
    Node* root = new Node(points[mid], split);

    vector<Point> left_points(points.begin(), points.begin() + mid);
    vector<Point> right_points(points.begin() + mid + 1, points.end());

    root->left = makeTree(left_points, depth + 1);
    root->right = makeTree(right_points, depth + 1);

    return root;
}

bool search(Node* root, Point target, int depth) {
    if (!root)
        return false;

    int split = root->split;
    if (root->p.val == target.val)
        return true;

    if (target.val[split] < root->p.val[split])
        return search(root->left, target, depth + 1);
    else
        return search(root->right, target, depth + 1);
}

int main() {
    vector<Point> points = {{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}};
    Node* root = makeTree(points, 0);

    Point target = {2, {9, 6}};
    if (search(root, target, 0))
        cout << "Target found!" << endl;
    else
        cout << "Target not found." << endl;

    return 0;
}


#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:

    const std::pair<int, int> axis[4] = {
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1}
    };

    struct PairHash {
        std::size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
        }
    };

    //unordered_map<std::pair<int, int>, int, PairHash> visited;

    int search(vector<vector<int>>& matrix, int i, int j, int val, vector<vector<int>>& visited)
    {
        //auto it = visited.find(std::make_pair(i, j));
        //if (it != visited.end()) {
        //    return it->second;
        //}

        if (visited[i][j] != -1) {
            return visited[i][j];
        }
             

        int longest = 1;
        for (auto ax : axis) {
            int current = 0;
            auto ii = i + ax.first;
            if (ii < 0 || matrix.size() <= ii) {
                continue;
            }

            auto jj = j + ax.second;
            if (jj < 0 || matrix[ii].size() <= jj) {
                continue;
            }

            int next = matrix[ii][jj];
            if (val < next) {
                current += 1;
                auto ret = search(matrix, ii, jj, next, visited);
                current += ret;

                visited[ii][jj] = ret;

                //visited.insert({ std::make_pair(ii, jj), ret });

                longest = std::max(longest, current);
            }
        }

        return longest;
    }

    int alg(vector<vector<int>>& matrix)
    {
        int longest = 0;
        vector<vector<int>> visited(matrix.size(), vector<int>(matrix[0].size(), -1));


        for (int i = 0; i < matrix.size(); i++) {
            auto& row = matrix[i];
            for (int j = 0; j < row.size(); j++) {
                auto current = search(matrix, i, j, row[j], visited);
                longest = std::max(longest, current);
            }
        }
        return longest;
    }

    int longestIncreasingPath(vector<vector<int>>& matrix) {
        return alg(matrix);
    }
}; 

int main()
{
    // [[9,9,4],[6,6,8],[2,1,1]]
    // [[3,4,5],[3,2,6],[2,2,1]]

    vector<vector<int>> matrix = {
        //{9,9,4},
        //{6,6,8},
        //{2,1,1}

        {3,4,5},
        {3,2,6},
        {2,2,1}
    };

    Solution a;

    auto b = a.longestIncreasingPath(matrix);


    return 1;
}


class Solution {
public:

    int mid1 = -1;
    int mid2 = -1;
    int save1 = -1;
    int save2 = -1;

    void alg(vector<int>& nums1, vector<int>& nums2, int i, int j)
    {
        if (mid2 == (i + j)) {
            return;
        }

        save2 = save1;

        if (i != nums1.size() && j != nums2.size()) {
            float ret = 0.0;
            auto a = nums1[i];
            auto b = nums2[j];

            if (b < a) {
               save1 = nums2[j];
               alg(nums1, nums2, i, j + 1);
            }
            else {
               save1 = nums1[i];
               alg(nums1, nums2, i + 1, j);
            }
        }
        else if (i < nums1.size()) {
            save1 = nums1[i];

            alg(nums1, nums2, i+1, j);
        } 
        else if (j < nums2.size()) {
            save1 = nums2[j];

            alg(nums1, nums2, i, j+1);
        }
    }

    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int size = nums1.size() + nums2.size();
        mid1 = size / 2.0;
        mid2 = mid1 + 1;

        alg(nums1, nums2, 0, 0);

        double ret = 0;
        if ((size % 2) == 0) {
            ret = (save1 + save2) / 2.0;
        }
        else {
            ret = save2 > save1 ? save2 : save1;
        }

        return ret;
    }
};

int main()
{
    Solution sol;

    vector<int> nums1{1,3};
    vector<int> nums2{2};

    //vector<int> nums1{1,2};
    //vector<int> nums2{3,4};

    //vector<int> nums1{};
    //vector<int> nums2{2,3};

    //vector<int> nums1{3};
    //vector<int> nums2{-2,-1};

    //vector<int> nums1{ 1 };
    //vector<int> nums2{ 1 };

    // tree?

    auto a = sol.findMedianSortedArrays(nums1, nums2);

    std::cout << "Hello World!\n";
}


1. 단순구현문제 골드까지 풀기(단계별)
한 문제를 시간제한없이 물고 늘어지지 말기
 - 시간제한을 정하여 해당시간 내에 답이 안나오면 과감히 답지를 보기(30분~1시간)
 - 이때, 가볍게 보고 넘기는것이 아니라 코드 한줄한줄을 완벽에 가깝게 이해하며 누군가에게 코드의 의도를 설명할 수 있을정도로 이해해야된다.

2. 복습문제 queue 관리
 - 답지를 보고 해결한 문제들을 모아놓고 특정 날이나 시간을 정해 그것들만 다시보자

3. 유형별로 풀기
 - 단계별로 풀기에 더해 알고리즘 유형별로 도장깨기 (단순구현 -> 완전탐색 -> 핵심 알고리즘 -> 복잡하고 어려운 알고리즘)