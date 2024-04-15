
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

using namespace std;

queue<pair<int, int>> q;
vector<vector<int>> map = {
    {0,0,1,1},
    {1,1,1,1},
    {2,2,2,1},
    {0,0,0,2}
};

vector<pair<int, int>> dir = {
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}
};

int maxX;
int maxY;

vector<int> result = { 0, 0, 0 };
set<pair<int, int>> visited;

void bfs(queue<pair<int, int>>& q)
{
    while (!q.empty()) {

        auto& k = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            auto x = k.first + dir[i].first;
            auto y = k.second + dir[i].second;

            if (visited.end() == visited.find(make_pair(x, y)))
                continue;
            if (x < maxX && y < maxY) {
                auto value = map[x][y];
                result[value]++;
                visited.emplace(make_pair(x,y));
                q.push(make_pair(x, y));
            }
        }
    }
}

int main()
{
    maxX = map.size();
    maxY = map[0].size();

    q.push(make_pair(0, 0));

    std::cout << "Hello World!\n";
}




