#include <vector>
#include <set>
#include <stack>
#include <iterator>

#define MAGAZINE_ITEERATOR for (int i = 0; i < MAGAZINE_HEIGHT; i++) for(int j = 0; j < MAGAZINE_HEIGHT; j++)
#define ORDER_ITERATOR for (int i = 0; i < ORDERS_AMOUNT; i++)

int ORDERS_AMOUNT = 1;
int MAGAZINE_HEIGHT = 10;
int MAGAZINE_WIDTH = 10;

enum Action;
struct Move;
struct Robot;
struct Position;
struct DFSStack;
struct DFSLevel;

void setRobotsAmount(int to);
void setMagazineSize(int height, int width);
vector<vector<Move>> simulateOrderCompletion(int **magazine, int robotPositions[][2], set<int> orders[]);

