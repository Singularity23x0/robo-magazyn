#include <vector>
#include <set>
#include <stack>

#define MAGAZINE_ITEERATOR for (int i = 0; i < MAGAZINE_HEIGHT; i++) for(int j = 0; j < MAGAZINE_HEIGHT; j++)

int ORDERS_AMOUNT = 1;
int MAGAZINE_HEIGHT = 10;
int MAGAZINE_WIDTH = 10;

enum Action;
struct Move;
struct Robot;
struct Position;

void setRobotsAmount(int to);
void setMagazineSize(int height, int width);
vector<vector<Move>> simulateOrderCompletion(int **products, int robotPositions[][2], set<int> orders[]);

