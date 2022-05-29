#include <algorithm>
#include <glog/logging.h>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <random>
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

    bool operator==(const Position &other)
    {
        return row == other.row && col == other.col;
    }

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

// method names are imposed by the library authors
void to_json(json &j, const Move &move);
void from_json(const json &j, Move &move);

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
        }
    };
}// namespace nlohmann