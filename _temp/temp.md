
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



