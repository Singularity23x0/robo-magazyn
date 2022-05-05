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
    bool equals(Position other)
    {
        return row == other.row && col == other.col;
    }
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

    bool initiateLevelReturn()
    {
        safetyReset();
        return topLevel.empty();
    }

    void safetyReset()
    {
        if(topLevel.empty() && levels.empty())
        {
            reset(topLevel.position);
        }
    }

    void returnOneLevel() 
    {
        topLevel = levels.back();
        levels.pop_back();
    }

    Position previousLevelPosition() 
    {
        return levels.back().position;
    }
};

struct Robot
{
    int id;
    int **magazine;
    Position *robotsPositions;
    set<int> order;
    bool orderComplete = false;
    DFSStack dfsStack;

    void init(int id, int **magazine, Position *robotsPositions, set<int> order) 
    {
        this->id = id;
        this->magazine = magazine;
        this->robotsPositions = robotsPositions;
        this->order = order;
        dfsStack.init(getPosition());
    }

    Position getPosition() 
    {
        return robotsPositions[id];
    }

    void setPosition(Position position)
    {
        robotsPositions[id] = position;
    }

    bool orderTurnedIn() 
    {
        return orderComplete && order.empty();
    }

    void sendToTurnInIfComplete()
    {
        if(!orderComplete && order.empty())
        {
            orderComplete = true;
            order.insert(ORDER_TURN_IN_STATION);
        }
    }

    Move makeMove() {
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
        else if (dfsStack.initiateLevelReturn()) 
        {
            Position target = dfsStack.previousLevelPosition();
            if(isPositionTaken(target, robotsPositions))
            {
                move.action = WAIT;
            }
            else 
            {
                nextPosition = target;
                move.action = defineMove(currentPosition, nextPosition);
                dfsStack.returnOneLevel();
            }
        }
        else 
        {
            // TODO: is there any available neighbor ? go there : do the WAIT action; 
        }

        setPosition(nextPosition);
        sendToTurnInIfComplete();
        return move;
    }
};

bool isPositionTaken(Position position, Position *robotsPositions)
{
    ORDER_ITERATOR
    {
        if(robotsPositions[i].equals(position))
            return true;
    }
    return false;
}

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

Action defineMove(Position from, Position to)
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
        dfs[i].init(i, magazine, robotPositions, orders[i]);
    }
    while(!simulationComplete)
    {
        // simulating all moves
        ORDER_ITERATOR
        {
            simulation[i].push_back(dfs[i].makeMove());
        }
        // checking if all simulations are complete
        simulationComplete = true;
        ORDER_ITERATOR
        {
            simulationComplete = simulationComplete && dfs[i].orderTurnedIn();
        }
    }

    return simulation;
}