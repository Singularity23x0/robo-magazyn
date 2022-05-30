#include "simulate.h"
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

int ORDERS_AMOUNT = 1;
int MAGAZINE_HEIGHT = 10;
int MAGAZINE_WIDTH = 10;
int ORDER_TURN_IN_STATION = -1;

bool Position::operator==(const Position &other)
{
    return row == other.row && col == other.col;
}
random_device dev;
mt19937 rng(dev());

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
    vector<Position> newNeighbors;
    for (std::size_t i = 0; i < neighbors.size(); i++)
        if (!(neighbors[i] == position)) {
            newNeighbors.push_back(neighbors[i]);
        }
    neighbors = newNeighbors;
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
    if (visited[position.row][position.col] < marker) {
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
        for (std::size_t i = 0; i < neighbors.size(); i++)
            if (!wasVisited(neighbors[i])) {
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
    if (topLevel.empty() && levels.empty()) {
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
    for (std::size_t i = 0; i < topLevel.neighbors.size(); i++) {
        Position *position = &topLevel.neighbors[i];
        bool isFree = true;
        for (int j = 0; j < ORDERS_AMOUNT && isFree; j++) {
            isFree = isFree && !(robotsPositions[j] == (*position));
        }
        if (isFree) {
            return position;
        }
    }
    return NULL;
}


void Robot::init(int id, vector<vector<int>> &magazine, Position *robotsPositions, Position endPosition, set<int> order)
{
    this->id = id;
    this->magazine = magazine;
    this->robotsPositions = robotsPositions;
    this->endPosition = endPosition;
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

void Robot::sendToTurnInIfComplete()
{
    if (!orderComplete && order.empty()) {
        orderComplete = true;
    }
}

Move Robot::makeMove()
{
    Move move;
    Position currentPosition = getPosition(), nextPosition = currentPosition;
    int availableProduct = magazine[currentPosition.row][currentPosition.col];
    move.position = currentPosition;

    set<int>::iterator it = order.find(availableProduct);

    if (orderComplete && currentPosition == endPosition) {
        reachedEnd = true;
        move.action = WAIT;// stay in the endPosition
    } else if (it != order.end()) {
        // TAKE action
        move.action = TAKE;
        order.erase(it);

        // resetting dfs
        dfsStack.reset(currentPosition);
    } else if (dfsStack.initiateLevelReturn()) {
        Position target = dfsStack.previousLevelPosition();
        if (isPositionTaken(target, robotsPositions)) {
            move.action = WAIT;
            if (waited)
                dfsStack.reset(currentPosition);
        } else {
            nextPosition = target;
            move.action = defineMove(currentPosition, nextPosition);
            dfsStack.returnOneLevel();
        }
    } else {
        Position *freeNeighbor = dfsStack.getFreeNeighbor(robotsPositions);
        if (freeNeighbor == NULL) {
            move.action = WAIT;
            if (waited)
                dfsStack.reset(currentPosition);
        } else {
            nextPosition.load(freeNeighbor);
            move.action = defineMove(currentPosition, nextPosition);
            dfsStack.visit(nextPosition);
        }
    }

    setPosition(nextPosition);
    sendToTurnInIfComplete();
    waited = move.action == WAIT; // to avoid waiting for a taken position for more than two iterations
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
        if (robotsPositions[i] == position)
            return true;
    }
    return false;
}

vector<Position> getNeighbors(Position currentPosition)
{
    int row = currentPosition.row, col = currentPosition.col;
    vector<Position> neighbors;
    if (row > 0) {
        neighbors.push_back(Position{row - 1, col});
    }
    if (row < MAGAZINE_HEIGHT - 1) {
        neighbors.push_back(Position{row + 1, col});
    }
    if (col > 0) {
        neighbors.push_back(Position{row, col - 1});
    }
    if (col < MAGAZINE_WIDTH - 1) {
        neighbors.push_back(Position{row, col + 1});
    }

    shuffle(begin(neighbors), end(neighbors), rng);
    return neighbors;
    return neighbors;
}

Action defineMove(Position from, Position to)
{
    if (from.row < to.row) {
        return GO_S;
    } else if (from.row > to.row) {
        return GO_N;
    } else if (from.col < to.col) {
        return GO_E;
    } else {
        return GO_W;
    }
}

vector<vector<Move>> simulate(vector<vector<int>> &magazine, Position robotPositions[], Position robotEndPositions[], set<int> orders[])
{
    srand(time(0));
    // LOG(INFO) << "Initializing simulation";
    vector<vector<Move>> simulation(ORDERS_AMOUNT);
    vector<Robot> dfs(ORDERS_AMOUNT);
    bool simulationComplete = false;
    ORDER_ITERATOR
    {
        dfs[i].init(i, magazine, robotPositions, robotEndPositions[i], orders[i]);
    }
    // LOG(INFO) << "Running simulation";
    int iteration = 0;
    int MAX_ITERATIONS = 1000;
    while (!simulationComplete) {
        // simulating all moves

        ORDER_ITERATOR
        {
            simulation[i].push_back(dfs[i].makeMove());
        }
        // checking if all simulations are complete
        simulationComplete = true;
        ORDER_ITERATOR
        {
            simulationComplete = simulationComplete && dfs[i].reachedEnd;
        }

        iteration++;
        if (iteration >= MAX_ITERATIONS) throw "MAX ITERATIONS EXCEEDED";
    }
    return simulation;
}


vector<vector<Move>> mutate(vector<vector<int>> &magazine, vector<vector<Move>> solution)
{
    uniform_int_distribution<mt19937::result_type> distribution(0, solution[0].size()-1);

    int from = distribution(rng);
    int to = distribution(rng);

    if (from == to) return solution;
    if (from > to) swap(from, to);

    // LOG(INFO) << "Mutation from: " << from << " to: " << to << endl;

    Position *robotPositions = new Position[ORDERS_AMOUNT];
    Position *robotEndPositions = new Position[ORDERS_AMOUNT];

    set<int> *orders = new set<int>[ORDERS_AMOUNT];

    ORDER_ITERATOR
    {
        Position pos = solution[i][from].position;
        robotPositions[i] = Position{pos.row, pos.col};

        pos = solution[i][to].position;
        robotEndPositions[i] = Position{pos.row, pos.col};

        for (int j = from; j <= to; j++) {
            if (solution[i][j].action == TAKE) {
                Position pos = solution[i][j].position;
                orders[i].insert(magazine[pos.row][pos.col]);
            }
        }
    }


    vector<vector<Move>> newPart = simulate(magazine, robotPositions, robotEndPositions, orders);
    vector<vector<Move>> newSolution(ORDERS_AMOUNT);

    // LOG(INFO) << "New fragment length: " << newPart[0].size() << ", previously: " << to - from + 1 << endl;

    // TODO: Check correct parts joining - should 'from' and 'to' be included and from which part
    ORDER_ITERATOR
    {
        for (int j = 0; j < from; j++) newSolution[i].push_back(solution[i][j]);
        for (int j = 0; j < newPart[0].size(); j++) newSolution[i].push_back(newPart[i][j]);
        for (int j = to; j < solution[0].size(); j++) newSolution[i].push_back(solution[i][j]);
    }

    delete[] robotPositions;
    delete[] robotEndPositions;
    delete[] orders;

    return newSolution;
}

// method names are imposed by the library authors
void to_json(json &j, const Move &move)
{
    j = json{
        {"row", move.position.row},
        {"col", move.position.col},
        {"action", move.action}};
}

void from_json(const json &j, Move &move)
{
    move.position.row = j.at("row").get<int>();
    move.position.col = j.at("col").get<int>();
    move.action = j.at("action").get<Action>();
}

// ALGORITHM

long Solution::size()
{
    return moves[0].size();
}

GeneticAlgorithm::GeneticAlgorithm(vector<vector<int>> &magazine, vector<Solution> population, int mutationsFromSolution)
{
    this->magazine = magazine;
    this->population = population;
    this->mutationsFromSolution = mutationsFromSolution;
}

void GeneticAlgorithm::run(int generationsAmount)
{
    init();
    for (int i = 0; i < generationsAmount; ++i) {
        newGeneration();
    }
}

void GeneticAlgorithm::init()
{
    topSolution = population[0];
    findBestSolution();
}

void GeneticAlgorithm::newGeneration()
{   
    mutateSolutions();
    findBestSolution();
    cout << "Current best: " << topSolution.size() << endl;
    pickNewPopulation();
}

void GeneticAlgorithm::mutateSolutions()
{
    int originalSize = population.size();
    for (int i = 0; i < originalSize; i++) {
        for (int j = 0; j < mutationsFromSolution; j++) {
            try {
                vector<vector<Move>> mutated = mutate(magazine, population[i].moves);

                population.push_back(Solution{mutated});
            } catch (...) {
                j -= 1;
            }
        }
    }
}

void GeneticAlgorithm::findBestSolution()
{
    for (int i = 0; i < population.size(); ++i) {
        if (population[i].size() < topSolution.size())
            topSolution = population[i];
    }
}

void GeneticAlgorithm::pickNewPopulation()
{
    int targetSize = population.size() / (mutationsFromSolution + 1);
    vector<Solution> newPopulation;
    vector<long> sizes;
    long maxSize = 0;
    long minSize = population[0].size();
    vector<int> weights;
    vector<bool> used(population.size(), false);

    for (int i = 0; i < population.size(); ++i) {
        sizes.push_back(population[i].size());
        maxSize = max(maxSize, population[i].size());
        minSize = min(minSize, population[i].size());
    }
    maxSize++;
    for (int i = 0; i < sizes.size(); ++i) {
        weights.push_back(maxSize - sizes[i]);
    }

    discrete_distribution<long> dist{weights.begin(), weights.end()};
    while (newPopulation.size() < targetSize) {
        int i = dist(rng);
        if (!used[i]) {
            used[i] = true;
            newPopulation.push_back(population[i]);
        }
    }
    population = newPopulation;
}

Solution *GeneticAlgorithm::bestSolution()
{
    return &topSolution;
}
