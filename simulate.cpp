#include <simulate.h>

using namespace std;

enum Action 
{
    WAIT = 0,
    GO_N = 1,
    GO_E = 2,
    GO_S = 3,
    GO_W = 4,
    TAKE = -1
};

struct Position
{
    int row, col;
};


struct Move
{
    Position position;
    Action action;
};

struct Robot
{
    int id;
    Position *positions;
    vector<int> order;
    vector<vector<bool>> visited;
    stack<vector<Position>> dfsStack;

    void init(int id, Position *positions, vector<int> order) 
    {
        this->id = id;
        this->order = order;
        this->positions = positions;
        visited.resize(MAGAZINE_HEIGHT, vector<bool>(MAGAZINE_WIDTH, false));
        Position position = getPosition();
        visited[position.row][position.col] = true;
    }

    Position getPosition() 
    {
        return positions[id];
    }

    Move makeMove(int robotPositions[][2]) {
        Move move;
        move.position = getPosition();

        // makes move; modifies robotPositions[id]

        return move;
    }
};

void setRobotsAmount(int to) 
{
    ORDERS_AMOUNT = to;
}

void setMagazineSize(int height, int width) 
{
    MAGAZINE_HEIGHT = height;
    MAGAZINE_WIDTH = width;
}

vector<vector<Move>> simulateOrderCompletion(int **products, Position robotPositions[], vector<int> orders[]) 
{
    vector<vector<Move>> simulation(ORDERS_AMOUNT);
    vector<Robot> dfs(ORDERS_AMOUNT);

    for(int i = 0; i < ORDERS_AMOUNT; i++) 
    {
        dfs[i].init(i, robotPositions, orders[i]);
    }

    return simulation;
}