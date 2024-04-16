
using namespace std;

template<typename Containter>
pair<int, int> getLocation(const Containter& container, int key) {

    auto it = container.find(key);
    if (it != container.end()) {

    }


}



#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <chrono>
#include <thread>

using namespace std;

queue<pair<int, int>> q;
vector<vector<int>> map = {
    //{0,0,1,1},
    //{1,1,1,1},
    //{2,2,2,1},
    //{0,0,0,2}
    {0, 0, 1},
    {2, 2, 1},
    {0, 0, 0}
};

vector<pair<int, int>> dir = {
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}
};

int maxX;
int maxY;
int current;

vector<int> result = { 0, 0, 0 };
set<pair<int, int>> visited;

void bfs(queue<pair<int, int>>& q)
{
    while (!q.empty()) {

        auto& k = q.front();
        q.pop();

        if (visited.end() == visited.find(make_pair(k.first, k.second)) &&
            k.first < maxX && k.second < maxY) {
            auto value = map[k.first][k.second];
            result[value]++;
            visited.emplace(make_pair(k.first, k.second));
            current = value;
        }

        for (int i = 0; i < 4; i++) {
            auto x = k.first + dir[i].first;
            auto y = k.second + dir[i].second;

            if (x < 0 || y < 0 || maxX <= x || maxY <= y)
                continue;
            if (visited.end() != visited.find(make_pair(x, y)))
                continue;

            auto value = map[x][y];
            if (current != value)
                continue;

            visited.emplace(make_pair(x, y));
            q.push(make_pair(x, y));
        }
    }
}



int main()
{
    maxX = map.size();
    maxY = map[0].size();

    for (int x = 0; x < map.size(); x++) {
        for (int y = 0; y < map.size(); y++) {

            if (x < 0 || y < 0 || maxX <= x || maxY <= y)
                continue;
            if (visited.end() != visited.find(make_pair(x, y)))
                continue;

            q.push(make_pair(x, y));

            bfs(q);
        }
    }


    

    vector<int> answer = { 2,1,2 };

    std::cout << "Hello World!\n";



    return 0;
}



// 0417.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <set>

using namespace std;

int main()
{
    vector<pair<int, int>> stars = {
        {21, 88},
        //{23, 75},
        //{97, 35},
        //{2, 8},
        //{67, 9},
        //{64, 75},
        {65, 71},
        //{70, 98},
        {9, 71},
        //{60, 35}
    };

    vector<set<int>> graph = { {21, 88}  };

    auto it = stars.begin();
    it++;

    for (; it != stars.end(); it++) {
        auto found = false;

        for (auto& group : graph) {
            auto it1 = group.find(it->first);
            auto it2 = group.find(it->second);
            auto it_end = group.end();

            if (it1 != it_end) {
                found = true;
                group.insert(it->second);
                break;
            }
            else if (it2 != it_end) {
                found = true;
                group.insert(it->second);
                break;
            }
        }

        if (!found) {
            graph.push_back({ it->first, it->second });
        }
    }

    std::cout << "Hello World!\n";
}
