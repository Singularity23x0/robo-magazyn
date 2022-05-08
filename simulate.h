#include <vector>
#include <set>
#include <stack>
#include <iterator>

#define MAGAZINE_ITEERATOR for (int i = 0; i < MAGAZINE_HEIGHT; i++) for(int j = 0; j < MAGAZINE_HEIGHT; j++)
#define ORDER_ITERATOR for (int i = 0; i < ORDERS_AMOUNT; i++)

int ORDERS_AMOUNT = 1;
int MAGAZINE_HEIGHT = 10;
int MAGAZINE_WIDTH = 10;
int ORDER_TURN_IN_STATION = -1;

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

void setRobotsAmount(int to);
void setMagazineSize(int height, int width);
vector<vector<Move>> simulate(vector<vector<int>> magazine, Position robotPositions[], set<int> orders[]);

