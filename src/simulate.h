#include <vector>
#include <set>
#include <stack>
#include <iterator>
#include <iostream>

using namespace std;

#define ORDER_ITERATOR for (int i = 0; i < ORDERS_AMOUNT; i++)

enum Action
{
    WAIT,
    GO_N,
    GO_E,
    GO_S,
    GO_W,
    TAKE
};
struct Position
{
    int row, col;
    bool equals(Position other);
    void load(Position *origin);
};
struct Move
{
    Position position;
    Action action;
};
struct DFSLevel 
{
    Position position;
    vector<Position> neighbors;
    bool empty();
    void remove(Position position);
};
struct DFSStack
{
    DFSLevel topLevel;
    vector<DFSLevel> levels;
    vector<vector<int>> visited;
    int marker;
    void init(Position position);
    void reset(Position position);
    void visit(Position position);
    bool wasVisited(Position field);
    bool initiateLevelReturn();
    void safetyReset();
    void returnOneLevel();
    Position previousLevelPosition();
    Position *getFreeNeighbor(Position *robotsPositions);
};
struct Robot
{
    int id;
    vector<vector<int>> magazine;
    Position *robotsPositions;
    set<int> order;
    bool orderComplete = false;
    DFSStack dfsStack;
    void init(int id, vector<vector<int>> &magazine, Position *robotsPositions, set<int> order);
    Position getPosition();
    void setPosition(Position position);
    bool orderTurnedIn();
    void sendToTurnInIfComplete();
    Move makeMove();
};

void setRobotsAmount(int to);
void setMagazineSize(int height, int width);
bool isPositionTaken(Position position, Position *robotsPositions);
vector<Position> getNeighbors(Position currentPosition);
Action defineMove(Position from, Position to);
vector<vector<Move>> simulate(vector<vector<int>> &magazine, Position robotPositions[], set<int> orders[]);

