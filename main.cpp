#include <iostream>
#include "simulate.h"

using namespace std;


int main(int argc, char const *argv[])
{
    setRobotsAmount(2);
    setMagazineSize(5, 5);
    vector<vector<int>> magazine = {
        {1, 1, 0, 1, 1},
        {2, 2, 0, 2, 2},
        {3, 3, 0, 3, 3},
        {4, 4, 0, 4, 4},
        {5, 5, 0, 5, 5}

    };
    Position robotPositions[2] = {
        Position {0, 0},
        Position {4, 4}
    };
    set<int> orders[2] = {
        set<int> {1, 3, 5, 0, 3, 5},
        set<int> {2, 3, 1, 1, 1, 1}
    };
    vector<vector<Move>> solution = simulate(magazine, robotPositions, orders);
    for(int i = 0; i < solution.size(); i++)
    {
        for(int j = 0; j < solution[i].size(); j++)
        {
            cout<<"("<<solution[i][j].position.row<<", "<<solution[i][j].position.col<<", "<<solution[i][j].action<<") ";
        }
        cout<<endl;
    }
    return 0;
}
