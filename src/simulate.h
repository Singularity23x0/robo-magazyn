#include <glog/logging.h>
#include <iterator>
#include <nlohmann/json.hpp>
#include <set>
#include <stack>
#include <vector>

using namespace std;
using json = nlohmann::json;

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

NLOHMANN_JSON_SERIALIZE_ENUM(Action, {{WAIT, "WAIT"},
                                      {GO_N, "N"},
                                      {GO_E, "E"},
                                      {GO_S, "S"},
                                      {GO_W, "W"},
                                      {TAKE, "TAKE"}})

struct Position {
    int row, col;

    bool operator==(const Position &other);
    void load(Position *origin);
};

struct Move {
    Position position;
    Action action;
};

struct DFSLevel {
    Position position;
    vector<Position> neighbors;
    bool empty();
    void remove(Position position);
};

struct DFSStack {
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
struct Robot {
    int id;
    vector<vector<int>> magazine;
    Position *robotsPositions;
    Position endPosition;
    set<int> order;
    bool orderComplete = false;
    bool reachedEnd = false;
    DFSStack dfsStack;
    void init(int id, vector<vector<int>> &magazine, Position *robotsPositions, Position endPosition, set<int> order);
    Position getPosition();
    void setPosition(Position position);
    bool orderTurnedIn();
    void sendToTurnInIfComplete();
    Move makeMove();
    bool waited = false;
};

void setRobotsAmount(int to);
void setMagazineSize(int height, int width);
bool isPositionTaken(Position position, Position *robotsPositions);
vector<Position> getNeighbors(Position currentPosition);
Action defineMove(Position from, Position to);
vector<vector<Move>> simulate(vector<vector<int>> &magazine, Position robotPositions[], Position robotEndPositions[], set<int> orders[]);

vector<vector<Move>> mutate(vector<vector<int>> &magazine, vector<vector<Move>> solution);

struct Configuration {
    vector<vector<int>> magazine;
    vector<Position> robotPositions;
    vector<Position> robotEndPositions;
    vector<set<int>> orders;
    int populationCount;
    int robotCount;
    int magazineWidth;
    int magazineHeight;
    int generationLimit;
    int mutationsFromSolution;
};

// method names are imposed by the library authors
void to_json(json &j, const Move &move);
void from_json(const json &j, Move &move);
void to_json(json &j, const Position &position);
void from_json(const json &j, Position &position);
void to_json(json &j, const Configuration &config);
void from_json(const json &j, Configuration &config);

namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::vector<std::vector<T>>> {
        static void to_json(json &j, const std::vector<std::vector<T>> &vec)
        {
            for (auto &row : vec) {
                j.push_back(json(row));
            }
        }

        static void from_json(const json &j, std::vector<std::vector<T>> &vec)
        {
            // not required for now
            using nlohmann::from_json;
            from_json(j, vec);
        }
    };
}// namespace nlohmann


// ALGORITHM
struct Solution {
    vector<vector<Move>> moves;
    long size();
};

class GeneticAlgorithm
{
private:
    vector<vector<int>> magazine;
    Solution topSolution;
    vector<Solution> population;
    int mutationsFromSolution;
    void init();
    void findBestSolution();
    void newGeneration();
    void mutateSolutions();
    void pickNewPopulation();

public:
    GeneticAlgorithm(vector<vector<int>> &magazine, vector<Solution> population, int mutationsFromSolution);
    void run(int generationsAmount);
    Solution *bestSolution();
    vector<int> generationBestScores;
    vector<int> generationWorstScores;
    vector<int> generationMedianScores;
};