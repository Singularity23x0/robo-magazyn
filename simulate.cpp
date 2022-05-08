#include <simulate.h>

using namespace std;

struct Position
{
    int row, col;
    bool equals(Position other)
    {
        return row == other.row && col == other.col;
    }

    void load(Position *origin)
    {
        row = origin->row;
        col = origin->col;
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

    void remove(Position position)
    {
        vector<Position> newNeighbiors;
        for(int i = 0; i < neighbors.size(); i++)
            if(!neighbors[i].equals(position))
            {
                newNeighbiors.push_back(neighbors[i]);
            }
        neighbors = newNeighbiors;
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

    void visit(Position position)
    {
        if(visited[position.row][position.col] < marker)
        {
            // remove current neighbor
            topLevel.remove(position);
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

    Position *getFreeNeighbor(Position *robotsPositions)
    {
        for(int i = 0; i < topLevel.neighbors.size(); i++)
        {
            Position *position = &topLevel.neighbors[i];
            bool isFree = true;
            for(int j = 0; j < ORDERS_AMOUNT && isFree; j++)
            {
                isFree = isFree && !robotsPositions[j].equals(*position);
            }
            if(isFree)
            {
                return position;
            }
        }
        return NULL;
    }
};

struct Robot
{
    int id;
    vector<vector<int>> magazine;
    Position *robotsPositions;
    set<int> order;
    bool orderComplete = false;
    DFSStack dfsStack;

    void init(int id, vector<vector<int>> &magazine, Position *robotsPositions, set<int> order) 
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
            Position *freeNeighbor = dfsStack.getFreeNeighbor(robotsPositions);
            if(freeNeighbor == NULL)
            {
                move.action = WAIT;
            }
            else 
            {
                nextPosition.load(freeNeighbor);
                move.action = defineMove(currentPosition, nextPosition);
                dfsStack.visit(nextPosition);
            }
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
    int row = currentPosition.row, col = currentPosition.col;
    vector<Position> neighbors;
    if(row > 0)
    {
        neighbors.push_back(Position {row - 1, col});
    }
    if(row < MAGAZINE_HEIGHT - 1)
    {
        neighbors.push_back(Position {row + 1, col});
    }
    if(col > 0)
    {
        neighbors.push_back(Position {row, col - 1});
    }
    if(col < MAGAZINE_WIDTH)
    {
        neighbors.push_back(Position {row, col + 1});
    }
    return neighbors;
}

Action defineMove(Position from, Position to)
{
    if(from.row < to.row)
    {
        return GO_S;
    }
    else if(from.row > to.row)
    {
        return GO_N;
    }
    else if(from.col < to.col)
    {
        return GO_E;
    }
    else 
    {
        return GO_W;
    }
}

vector<vector<Move>> simulate(vector<vector<int>> &magazine, Position robotPositions[], set<int> orders[])
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