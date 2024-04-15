
using namespace std;

template<typename Containter>
pair<int, int> getLocation(const Containter& container, int key) {

    auto it = container.find(key);
    if (it != container.end()) {

    }


}


int main()
{
    // 1, 4, 7, *
    // 2, 5, 8, 0
    // 3, 6, 9, *

    //vector<vector<int>>(friends.size(), vector<int>(friends.size(), 0));

    // 7 -> {0, 2}
    //[1, 3, 4, 5, 8, 2, 1, 4, 5, 9, 5]	"right"	"LRLLLRLLRRL"


    vector<int> keyPad = { 1, 4, 7, -1, 2, 5, 8, 0, 3, 6, 9, -2};
    vector<int> input = { 1, 3, 4, 5, 8, 2, 1, 4, 5, 9, 5 };

    map<int, pair<int, int>> keyPadMap;
    set<int> leftSet = { 1,4,7,-1 };
    set<int> rightSet = { 3,6,9,-2 };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            keyPadMap.emplace(keyPad[i*4 + j], make_pair(i, j));
        }
    }

    int left = -1;
    int right = -2;

    auto lookUp = [](int key) -> void {

    };

    for (auto in : input) {


        leftSet.find(in);


        auto left_it = keyPadMap.find(left);
        auto right_it = keyPadMap.find(right);

        auto it = keyPadMap.find(in);

        std::cout << it->second.first << "," << it->second.second << endl;
    }

    


    return 0;
    
}


