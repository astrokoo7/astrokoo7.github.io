
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

