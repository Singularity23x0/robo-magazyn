#include <iostream>
#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "simulate.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    google::InitGoogleLogging(argv[0]);
    fLB::FLAGS_logtostderr = true;

    LOG(INFO) << "Running simulation...";
    

    setRobotsAmount(4);
    setMagazineSize(6, 5);
    vector<vector<int>> magazine = {
        {-1, 1, 0, 1, -1},
        {2, 2, 0, 2, 2},
        {3, 3, 0, 3, 3},
        {4, 4, 0, 4, 4},
        {5, 5, 0, 5, 5},
        {-1, 2, 3, 4, -1}

    };
    Position robotPositions[4] = {
        Position {0, 0},
        Position {4, 4},
        Position {2, 4},
        Position {4, 1}
    };
    set<int> orders[4] = {
        set<int> {0, 1, 3},
        set<int> {0, 1, 5},
        set<int> {1, 3, 4},
        set<int> {2, 4}
    };
    vector<vector<Move>> solution = simulate(magazine, robotPositions, orders);

    LOG(INFO) << "Converting solution to JSON format";
    json j = solution;
    LOG(INFO) << "Writing solution in JSON format to stdout";
    std::cout << j << std::endl;

    // for(std::size_t i = 0; i < solution.size(); i++)
    // {
    //     for(std::size_t j = 0; j < solution[i].size(); j++)
    //     {
    //         cout<< "(" << solution[i][j].position.row << ", " <<solution[i][j].position.col << ", " <<solution[i][j].action<< ") ";
    //     }
    //     cout << endl;
    // }
    return 0;
}
