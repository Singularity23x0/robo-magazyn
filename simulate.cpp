#include <simulate.h>

using namespace std;

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

    bool empty()
    {
        return neighbors.empty();
    }
};

struct DFSStack
{
    DFSLevel topLevel;
    vector<DFSLevel> levels;
    vector<vector<int>> visited;
    int marker; // marks visited; if < marker then not visited in this run; each reset = new run = marker++

    void init(Position position)
    {
        visited.resize(MAGAZINE_HEIGHT, vector<int>(MAGAZINE_WIDTH, 0));
        marker = 0;
        reset(position);
    }

    void reset(Position position)
    {
        levels.clear();
        marker++;
        visited[position.row][position.col] = marker;
        topLevel.position = position;
        topLevel.neighbors = getNeighbors(position);
    }

    void addLevel(Position position)
    {
        // set position as visited
        visited[position.row][position.col] = marker;
        // push old level into memory
        levels.push_back(topLevel);
        // list neighbors
        vector<Position> neighbors = getNeighbors(position);
        // create and set new top level
        DFSLevel newLevel;
        newLevel.position = position;
        for(int i = 0; i < neighbors.size(); i++)
            if(!wasVisited(neighbors[i]))
            {
                newLevel.neighbors.push_back(neighbors[i]);
            }
        topLevel = newLevel;
    }

    bool wasVisited(Position field)
    {
        return visited[field.row][field.col] >= marker;
    }

    bool topLevelEmpty()
    {
        return topLevel.empty();
    }
};

struct Robot
{
    int id;
    Position *positions;
    set<int> order;
    DFSStack dfsStack;

    void init(int id, Position *positions, set<int> order) 
    {
        this->id = id;
        this->order = order;
        this->positions = positions;
        Position position = getPosition();
        dfsStack.init(position);
        // TODO: add base level to dfs stack
    }

    Position getPosition() 
    {
        return positions[id];
    }

    bool isComplete() {
        return order.empty();
    }

    Move makeMove(int **magazine, Position robotPositions[]) {
        Move move;
        Position currentPosition=getPosition(), nextPosition=currentPosition;
        int availableProduct = magazine[currentPosition.row][currentPosition.col];
        move.position = currentPosition;

        set<int>::iterator it = order.find(availableProduct);
        if (it != order.end()) 
        {
            // TAKE action
            move.action = TAKE;
            order.erase(it);

            // resetting dfs
            dfsStack.reset(currentPosition);
        }
        else if (dfsStack.topLevelEmpty()) 
        {
            // TODO: can return to previous ? return : do the WAIT action;
        }
        else 
        {
            // TODO: is there any available neighbor ? go there : do the WAIT action; 
        }

        robotPositions[id] = nextPosition;
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

vector<Position> getNeighbors(Position currentPosition)
{
    // TODO: implement
}

vector<vector<Move>> simulateOrderCompletion(int **magazine, Position robotPositions[], set<int> orders[]) 
{
    vector<vector<Move>> simulation(ORDERS_AMOUNT);
    vector<Robot> dfs(ORDERS_AMOUNT);
    bool simulationComplete = false;
    ORDER_ITERATOR
    {
        dfs[i].init(i, robotPositions, orders[i]);
    }
    while(!simulationComplete)
    {
        // simulating all moves
        ORDER_ITERATOR
        {
            simulation[i].push_back(dfs[i].makeMove(magazine, robotPositions));
        }
        // checking if all simulations are complete
        simulationComplete = true;
        ORDER_ITERATOR
        {
            simulationComplete = simulationComplete && dfs[i].isComplete();
        }
    }

    return simulation;
}