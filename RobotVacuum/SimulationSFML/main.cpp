#include <SFML/Graphics.hpp>
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

class Item {
public:
    static Item* createItem() {
        return new Item();
    }

    ~Item() {}

    __UINT8_FAST_LOCATION__ location;

    Item() : location{0, 0} {}
};

const uint8_t GRID_SIZE_X = 20;
const uint8_t GRID_SIZE_Y = 20;
const int CELL_SIZE = 30; // Size of each cell in pixels

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

int Heuristic(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

vector<__UINT8_FAST_LOCATION__> AStar(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ goal) {
    priority_queue<Node, vector<Node>, greater<Node>> openList;

    bool closedList[GRID_SIZE_X][GRID_SIZE_Y] = { false };

    openList.push(Node(start.x, start.y, 0, Heuristic(start, goal), nullptr));

    vector<__UINT8_FAST_LOCATION__> path;

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        // Hedefe ulaşıldıysa yol oluşturuluyor
        if (current.x == goal.x && current.y == goal.y) {
            Node* node = &current;
            while (node != nullptr) {
                path.push_back({node->x, node->y});
                node = node->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Şu anki düğüm kapalı listeye ekleniyor
        closedList[current.x][current.y] = true;

        // 8 yönlü hareket: sağ, sol, yukarı, aşağı ve çapraz yönler
        vector<pair<int, int>> directions = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},   // Dikey ve yatay
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // Çapraz
        };

        // Komşu düğümler kontrol ediliyor
        for (auto& dir : directions) {
            int newX = current.x + dir.first;
            int newY = current.y + dir.second;

            // Yeni pozisyonun geçerli olup olmadığını kontrol ediyoruz
            if (newX >= 0 && newX < GRID_SIZE_X && newY >= 0 && newY < GRID_SIZE_Y && !closedList[newX][newY] && !obstacleGrid[newX][newY]) {
                int newG = current.g + ((dir.first != 0 && dir.second != 0) ? 14 : 10); // Dikey ve yatay için 10, çapraz için 14 (yaklaşık kök 2)
                int newH = Heuristic({(uint_fast8_t)newX, (uint_fast8_t)newY}, goal);
                openList.push(Node(newX, newY, newG, newH, new Node(current)));
            }
        }
    }

    // Eğer yol bulunamazsa boş bir liste döner
    return path;
}

// A* algoritmasıyla bulunan yolu takip ederek hareket ettirme
void FollowPath(Item& cleaner, vector<__UINT8_FAST_LOCATION__>& path, sf::RenderWindow& window) {
    for (auto& step : path) {
        cleaner.location.x = step.x;
        cleaner.location.y = step.y;

        // Clear the window
        window.clear();

        // Draw the grid
        for (uint_fast8_t x = 0; x < GRID_SIZE_X; x++) {
            for (uint_fast8_t y = 0; y < GRID_SIZE_Y; y++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);

                if (x == cleaner.location.x && y == cleaner.location.y) {
                    cell.setFillColor(sf::Color::Green); // Cleaner
                } else if (obstacleGrid[x][y]) {
                    cell.setFillColor(sf::Color::Red); // Obstacle
                } else if (garbageGrid[x][y]) {
                    cell.setFillColor(sf::Color::Yellow); // Garbage
                } else {
                    cell.setFillColor(sf::Color::White); // Empty
                }
                window.draw(cell);
            }
        }

        // Display the contents of the window
        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(75)); // FPS
        
        // Çöp toplandıysa, çöp grid'den kaldır
        if (garbageGrid[cleaner.location.x][cleaner.location.y]) {
            garbageGrid[cleaner.location.x][cleaner.location.y] = false; // Remove garbage
        }
    }
}

int main(void)
{
    Item* cleaner = Item::createItem(); 

    int numGarbage = 30; 
    int numObstacles = 20; 
    PlaceGarbage(numGarbage);
    PlaceObstacle(numObstacles);

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE_X * CELL_SIZE, GRID_SIZE_Y * CELL_SIZE), "Garbage Collector");

    // Temizleyiciyi çöplere yönlendir ve her çöp için A* ile yol bul
    for (int x = 0; x < GRID_SIZE_X; x++) {
        for (int y = 0; y < GRID_SIZE_Y; y++) {
            if (garbageGrid[x][y]) {
                __UINT8_FAST_LOCATION__ garbageLocation = { (uint_fast8_t)x, (uint_fast8_t)y };
                vector<__UINT8_FAST_LOCATION__> path = AStar(cleaner->location, garbageLocation);
                if (!path.empty()) {
                    FollowPath(*cleaner, path, window);
                }
            }
        }
    }

    // Geri başlangıç noktasına dön
    __UINT8_FAST_LOCATION__ origin = {0, 0};
    vector<__UINT8_FAST_LOCATION__> returnPath = AStar(cleaner->location, origin);
    if (!returnPath.empty()) {
        FollowPath(*cleaner, returnPath, window);
    }

    cout << "\nReturned to the Beginning -> X: " << static_cast<int>(cleaner->location.x) << " Y: " << static_cast<int>(cleaner->location.y) << endl;

    delete cleaner; 

    return 0;
}
