#include <iostream>
#include <cstdint>
#include <thread> 
#include <chrono> 
#include <cstdlib> 
#include <ctime> 
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>


using namespace std;

struct __UINT8_FAST_LOCATION__
{
    uint_fast8_t x;
    uint_fast8_t y;
};

struct __UINT8_FAST_AXIS__
{
    bool moveX; 
    bool moveY; 
    bool movePositiveX; // Direction: true = positive, false = negative
    bool movePositiveY; // Direction: true = positive, false = negative
};

__UINT8_FAST_AXIS__ axis = {false, false, false, false};

class Item {
public:
    static Item* createItem() {
        return new Item();
    }

    ~Item() {
      
    }

    __UINT8_FAST_LOCATION__ location;
    
    Item() : location{0, 0} {} 
};

const uint8_t GRID_SIZE_X = 20;
const uint8_t GRID_SIZE_Y = 20;

bool garbageGrid[GRID_SIZE_X][GRID_SIZE_Y] = { false };
bool obstacleGrid[GRID_SIZE_X][GRID_SIZE_Y] = { false };

// A* için düğüm yapısı
struct Node {
    uint_fast8_t x, y;
    int g, h; // g: hareket maliyeti, h: hedefe tahmini maliyet
    Node* parent;
    
    Node(uint_fast8_t _x, uint_fast8_t _y, int _g, int _h, Node* _parent) 
        : x(_x), y(_y), g(_g), h(_h), parent(_parent) {}
    
    int f() const { return g + h; }
    
    bool operator>(const Node& other) const { return f() > other.f(); }
};

void PlaceGarbage(int numGarbage)
{
    srand(static_cast<unsigned>(time(0))); 
    for (int i = 0; i < numGarbage; i++)
    {
        int x = rand() % GRID_SIZE_X;
        int y = rand() % GRID_SIZE_Y;
        garbageGrid[x][y] = true; 
    }
}

void PlaceObstacle(int numObstacles)
{
    srand(static_cast<unsigned>(time(0) + 1));
    for (int i = 0; i < numObstacles; i++)
    {
        int x = rand() % GRID_SIZE_X;
        int y = rand() % GRID_SIZE_Y;
        obstacleGrid[x][y] = true; 
    }
}

void Wait(uint16_t milliSeconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds)); 
}

void PrintGrid(Item &cleaner)
{
    system("cls"); 

    for (uint_fast8_t _x = 0; _x < GRID_SIZE_X; _x++)
    {
        for (uint_fast8_t _y = 0; _y < GRID_SIZE_Y; _y++)
        {
            if (_x == cleaner.location.x && _y == cleaner.location.y)
            {
                cout << " O"; 
                if (garbageGrid[_x][_y])
                {
                    garbageGrid[_x][_y] = false; 
                }
            }
            else if (obstacleGrid[_x][_y])
            {
                cout << "[]";
            }
            else if (garbageGrid[_x][_y])
            {
                cout << " #";
            }
            else
            {
                cout << " ."; 
            }
        }
        cout << "\n"; 
    }
}

// Manhattan mesafesi hesaplama
int Heuristic(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

// A* algoritması ile yol bulma
vector<__UINT8_FAST_LOCATION__> AStar(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ goal) {
    priority_queue<Node, vector<Node>, greater<Node>> openList;
    bool closedList[GRID_SIZE_X][GRID_SIZE_Y] = { false };
    
    openList.push(Node(start.x, start.y, 0, Heuristic(start, goal), nullptr));

    vector<__UINT8_FAST_LOCATION__> path;

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();
        
        if (current.x == goal.x && current.y == goal.y) {
            Node* node = &current;
            while (node != nullptr) {
                path.push_back({node->x, node->y});
                node = node->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        closedList[current.x][current.y] = true;

        vector<pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // Dikey ve yatay yönler

        for (auto& dir : directions) {
            int newX = current.x + dir.first;
            int newY = current.y + dir.second;

            if (newX >= 0 && newX < GRID_SIZE_X && newY >= 0 && newY < GRID_SIZE_Y && !closedList[newX][newY] && !obstacleGrid[newX][newY]) {
                int newG = current.g + 1;
                int newH = Heuristic({(uint_fast8_t)newX, (uint_fast8_t)newY}, goal);
                openList.push(Node(newX, newY, newG, newH, new Node(current)));
            }
        }
    }

    return path; // Yol bulunamazsa boş bir liste döner
}

// A* algoritmasıyla bulunan yolu takip ederek hareket ettirme
void FollowPath(Item& cleaner, vector<__UINT8_FAST_LOCATION__>& path) {
    for (auto& step : path) {
        cleaner.location.x = step.x;
        cleaner.location.y = step.y;
        PrintGrid(cleaner);
        Wait(100);
    }
}

int main(void)
{
    Item* cleaner = Item::createItem(); 

    int numGarbage = 30; 
    int numObstacles = 20; 
    PlaceGarbage(numGarbage);
    PlaceObstacle(numObstacles);

    // Temizleyiciyi çöplere yönlendir ve her çöp için A* ile yol bul
    for (int x = 0; x < GRID_SIZE_X; x++) {
        for (int y = 0; y < GRID_SIZE_Y; y++) {
            if (garbageGrid[x][y]) {
                __UINT8_FAST_LOCATION__ garbageLocation = { (uint_fast8_t)x, (uint_fast8_t)y };
                vector<__UINT8_FAST_LOCATION__> path = AStar(cleaner->location, garbageLocation);
                if (!path.empty()) {
                    FollowPath(*cleaner, path);
                }
            }
        }
    }

    // Geri başlangıç noktasına dön
    __UINT8_FAST_LOCATION__ origin = {0, 0};
    vector<__UINT8_FAST_LOCATION__> returnPath = AStar(cleaner->location, origin);
    if (!returnPath.empty()) {
        FollowPath(*cleaner, returnPath);
    }

    cout << "\nReturned to the Beginning -> X: " << static_cast<int>(cleaner->location.x) << " Y: " << static_cast<int>(cleaner->location.y) << endl;

    delete cleaner; 
    return 0;
}
