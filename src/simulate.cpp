#include "simulate.h"
#include <glog/logging.h>

using namespace std;

int ORDERS_AMOUNT = 1;
int MAGAZINE_HEIGHT = 10;
int MAGAZINE_WIDTH = 10;
int ORDER_TURN_IN_STATION = -1;

void Position::load(Position *origin)
{
    row = origin->row;
    col = origin->col;
}

bool DFSLevel::empty()
{
    return neighbors.empty();
}

void DFSLevel::remove(Position position)
{
    vector<Position> newNeighbiors;
    for (std::size_t i = 0; i < neighbors.size(); i++)
        if (!(neighbors[i] == position))
        {
            newNeighbiors.push_back(neighbors[i]);
        }
    neighbors = newNeighbiors;
}


void DFSStack::init(Position position)
{
    visited.resize(MAGAZINE_HEIGHT, vector<int>(MAGAZINE_WIDTH, 0));
    marker = 0;
    reset(position);
}

void DFSStack::reset(Position position)
{
    levels.clear();
    marker++;
    visited[position.row][position.col] = marker;
    topLevel.position = position;
    topLevel.neighbors = getNeighbors(position);
}

void DFSStack::visit(Position position)
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
        for(std::size_t i = 0; i < neighbors.size(); i++)
            if(!wasVisited(neighbors[i]))
            {
                newLevel.neighbors.push_back(neighbors[i]);
            }
        topLevel = newLevel;
    }
}

bool DFSStack::wasVisited(Position field)
{
    return visited[field.row][field.col] >= marker;
}

bool DFSStack::initiateLevelReturn()
{
    safetyReset();
    return topLevel.empty();
}

void DFSStack::safetyReset()
{
    if(topLevel.empty() && levels.empty())
    {
        reset(topLevel.position);
    }
}

void DFSStack::returnOneLevel() 
{
    topLevel = levels.back();
    levels.pop_back();
}

Position DFSStack::previousLevelPosition() 
{
    return levels.back().position;
}

Position *DFSStack::getFreeNeighbor(Position *robotsPositions)
{
    for(std::size_t i = 0; i < topLevel.neighbors.size(); i++)
    {
        Position *position = &topLevel.neighbors[i];
        bool isFree = true;
        for(int j = 0; j < ORDERS_AMOUNT && isFree; j++)
        {
            isFree = isFree && !(robotsPositions[j] == (*position));
        }
        if (isFree)
        {
            return position;
        }
    }
    return NULL;
}


void Robot::init(int id, vector<vector<int>> &magazine, Position *robotsPositions, set<int> order) 
{
    this->id = id;
    this->magazine = magazine;
    this->robotsPositions = robotsPositions;
    this->order = order;
    dfsStack.init(getPosition());
}

Position Robot::getPosition() 
{
    return robotsPositions[id];
}

void Robot::setPosition(Position position)
{
    robotsPositions[id] = position;
}

bool Robot::orderTurnedIn() 
{
    return orderComplete && order.empty();
}

void Robot::sendToTurnInIfComplete()
{
    if(!orderComplete && order.empty())
    {
        orderComplete = true;
        order.insert(ORDER_TURN_IN_STATION);
    }
}

Move Robot::makeMove() 
{
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
    // cout<<id<<": "<<"{"<<move.action<<", "<<move.position.row<<", "<<move.position.col<<"}"<<endl;
    return move;
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

bool isPositionTaken(Position position, Position *robotsPositions)
{
    ORDER_ITERATOR
    {
        if(robotsPositions[i] == position)
            return true;
    }
    return false;
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
    if(col < MAGAZINE_WIDTH - 1)
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
    LOG(INFO) << "Initializing simulation";
    vector<vector<Move>> simulation(ORDERS_AMOUNT);
    vector<Robot> dfs(ORDERS_AMOUNT);
    bool simulationComplete = false;
    ORDER_ITERATOR
    {
        dfs[i].init(i, magazine, robotPositions, orders[i]);
    }
    LOG(INFO) << "Running simulation";
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

// method names are imposed by the library authors
void to_json(json &j, const Move &move) {
    j = json {
        { "row", move.position.row },
        { "col", move.position.col },
        { "action", move.action }
    };
}

void from_json(const json &j, Move &move) {
    move.position.row = j.at("row").get<int>();
    move.position.col = j.at("col").get<int>();
    move.action = j.at("action").get<Action>();
}
