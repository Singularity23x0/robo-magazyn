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

bool Position::operator!=(const Position &other)
{
	return row != other.row || col != other.col;
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
		std::copy_if(std::begin(neighbors),
								 std::end(neighbors),
								 std::back_inserter(newNeighbors),
								 [&position](Position &pos){return pos != position;});
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
    ORDER_ITERATOR(i)
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
    vector<vector<Move>> simulation(ORDERS_AMOUNT);
    vector<Robot> dfs(ORDERS_AMOUNT);
    bool simulationComplete = false;
    ORDER_ITERATOR(i)
    {
        dfs[i].init(i, magazine, robotPositions, robotEndPositions[i], orders[i]);
    }
    int iteration = 0;
    int MAX_ITERATIONS = 1000;
    while (!simulationComplete) {
        // simulating all moves

        ORDER_ITERATOR(i)
        {
            simulation[i].push_back(dfs[i].makeMove());
        }
        // checking if all simulations are complete
        simulationComplete = true;
        ORDER_ITERATOR(i)
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
    uniform_int_distribution<mt19937::result_type> distribution(0, solution[0].size() - 1);

    int from = distribution(rng);
    int to = distribution(rng);

    if (from == to) return solution;
    if (from > to) swap(from, to);

		std::vector<Position> robotPositions(ORDERS_AMOUNT);
		std::vector<Position> robotEndPositions(ORDERS_AMOUNT);
		std::vector<set<int>> orders(ORDERS_AMOUNT);

    ORDER_ITERATOR(i)
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


    vector<vector<Move>> newPart = simulate(magazine, robotPositions.data(), robotEndPositions.data(), orders.data());
    vector<vector<Move>> newSolution(ORDERS_AMOUNT);

    // TODO: Check correct parts joining - should 'from' and 'to' be included and from which part
    ORDER_ITERATOR(i)
    {
        for (int j = 0; j < from; j++) newSolution[i].push_back(solution[i][j]);
        for (std::size_t j = 0; j < newPart[0].size(); j++) newSolution[i].push_back(newPart[i][j]);
        for (std::size_t j = to; j < solution[0].size(); j++) newSolution[i].push_back(solution[i][j]);
    }

    return newSolution;
}

// method names are imposed by the library authors
void to_json(json &j, const Move &move)
{
    j = json{
        {"row", move.position.row},
        {"col", move.position.col},
        {"action", move.action}
    };
}

void from_json(const json &j, Move &move)
{
    move.position.row = j.at("row").get<int>();
    move.position.col = j.at("col").get<int>();
    move.action = j.at("action").get<Action>();
}

void to_json(json &j, const Position &position)
{
	j = json {
		{"row", position.row},
		{"col", position.col}
	};
}

void from_json(const json &j, Position &position)
{
	position.row = j.at("row").get<int>();
	position.col = j.at("col").get<int>();
}

void to_json(json &j, const Configuration &config)
{
    j = json {
        {"magazine", config.magazine},
        {"robotPositions", config.robotPositions},
        {"robotEndPositions", config.robotEndPositions},
        {"orders", config.orders},
        {"initialPopulationSize", config.initialPopulationSize},
        {"robotCount", config.robotCount},
        {"magazineWidth", config.magazineWidth},
        {"magazineHeight", config.magazineHeight},
				{"generationLimit", config.generationLimit},
				{"mutationsFromSolution", config.mutationsFromSolution}
    };
}

void from_json(const json &j, Configuration &config)
{
    config.magazine = j.at("magazine").get<vector<vector<int>>>();
    config.robotPositions = j.at("robotPositions").get<vector<Position>>();
    config.robotEndPositions = j.at("robotEndPositions").get<vector<Position>>();
    config.orders = j.at("orders").get<vector<set<int>>>();
    config.initialPopulationSize = j.at("initialPopulationSize").get<int>();
    config.robotCount = j.at("robotCount").get<int>();
    config.magazineWidth = j.at("magazineWidth").get<int>();
    config.magazineHeight = j.at("magazineHeight").get<int>();
		config.generationLimit = j.at("generationLimit").get<int>();
		config.mutationsFromSolution = j.at("mutationsFromSolution").get<int>();
}

// ALGORITHM

long Solution::size() const
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
        newGeneration(i);
    }
}

void GeneticAlgorithm::init()
{
    topSolution = population[0];
    findBestSolution();
}

void GeneticAlgorithm::newGeneration(int generationNumber)
{   
    mutateSolutions();
    findBestSolution();
		LOG(INFO) << "Generation no. " << generationNumber << ", best: " << topSolution.size();

		auto minInPopulation = std::min_element(
			std::begin(population),
			std::end(population),
			[](const Solution &sol1, const Solution &sol2){
				return sol1.size() < sol2.size();
			}
		);

    generationBestScores.push_back(minInPopulation->size());

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
	topSolution = *std::min_element(
		std::begin(population),
		std::end(population),
		[](const Solution &sol1, const Solution &sol2) {
			return sol1.size() < sol2.size();
		}
	);
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

		// TODO: consider using minmax algorithm from stdlib
    for (std::size_t i = 0; i < population.size(); ++i) {
        sizes.push_back(population[i].size());
        maxSize = max(maxSize, population[i].size());
        minSize = min(minSize, population[i].size());
    }
    maxSize++;
    for (std::size_t i = 0; i < sizes.size(); ++i) {
        weights.push_back(maxSize - sizes[i]);
    }

    discrete_distribution<long> dist{weights.begin(), weights.end()};
    while ((int)newPopulation.size() < targetSize) {
        int i = dist(rng);
        if (!used[i]) {
            used[i] = true;
            newPopulation.push_back(population[i]);
        }
    }
    population = newPopulation;
}

Solution GeneticAlgorithm::bestSolution()
{
    return topSolution;
}
