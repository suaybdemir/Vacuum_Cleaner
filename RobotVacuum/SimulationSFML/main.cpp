#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <cmath>
#include <algorithm>

const int GRID_SIZE_X = 20;
const int GRID_SIZE_Y = 20;
const int CELL_SIZE = 30;
const int MOVE_DELAY_MS = 200; // Her hareketten sonra bekleme süresi (ms)

struct __UINT8_FAST_LOCATION__ {
    uint_fast8_t x;
    uint_fast8_t y;
};

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

// A* algoritması için manhattan mesafesi
int Heuristic(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

// A* algoritması ile yol bulma
std::vector<__UINT8_FAST_LOCATION__> AStar(__UINT8_FAST_LOCATION__ start, __UINT8_FAST_LOCATION__ goal, bool obstacleGrid[GRID_SIZE_X][GRID_SIZE_Y]) {
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    bool closedList[GRID_SIZE_X][GRID_SIZE_Y] = { false };

    openList.push(Node(start.x, start.y, 0, Heuristic(start, goal), nullptr));
    std::vector<__UINT8_FAST_LOCATION__> path;

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        if (current.x == goal.x && current.y == goal.y) {
            Node* node = &current;
            while (node != nullptr) {
                path.push_back({ node->x, node->y });
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedList[current.x][current.y] = true;

        std::vector<std::pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

        for (auto& dir : directions) {
            int newX = current.x + dir.first;
            int newY = current.y + dir.second;

            if (newX >= 0 && newX < GRID_SIZE_X && newY >= 0 && newY < GRID_SIZE_Y && !closedList[newX][newY] && !obstacleGrid[newX][newY]) {
                int newG = current.g + 1;
                int newH = Heuristic({ (uint_fast8_t)newX, (uint_fast8_t)newY }, goal);
                openList.push(Node(newX, newY, newG, newH, new Node(current)));
            }
        }
    }

    return path; // Yol bulunamazsa boş bir liste döner
}

// En yakın çöpü bulma fonksiyonu
__UINT8_FAST_LOCATION__ FindNearestGarbage(__UINT8_FAST_LOCATION__ start, bool garbageGrid[GRID_SIZE_X][GRID_SIZE_Y]) {
    for (int dist = 1; dist < GRID_SIZE_X + GRID_SIZE_Y; dist++) { // 1'den başlayarak en yakın çöpü bul
        for (int dx = -dist; dx <= dist; dx++) {
            for (int dy = -dist; dy <= dist; dy++) {
                if (abs(dx) + abs(dy) == dist) { // Manhattan mesafesi
                    int newX = start.x + dx;
                    int newY = start.y + dy;
                    if (newX >= 0 && newX < GRID_SIZE_X && newY >= 0 && newY < GRID_SIZE_Y && garbageGrid[newX][newY]) {
                        return { (uint_fast8_t)newX, (uint_fast8_t)newY }; // En yakın çöp
                    }
                }
            }
        }
    }
    return { 255, 255 }; // Eğer çöp yoksa geçersiz bir konum döner
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow window(sf::VideoMode(GRID_SIZE_X * CELL_SIZE, GRID_SIZE_Y * CELL_SIZE), "Cleaner Simulation");

    bool garbageGrid[GRID_SIZE_X][GRID_SIZE_Y] = { false };
    bool obstacleGrid[GRID_SIZE_X][GRID_SIZE_Y] = { false };

    __UINT8_FAST_LOCATION__ cleanerLocation = { 0, 0 };
    __UINT8_FAST_LOCATION__ startLocation = cleanerLocation; // Başlangıç konumunu sakla

    // Çöp ve engel yerleştirme
    for (int i = 0; i < 30; i++) {
        garbageGrid[rand() % GRID_SIZE_X][rand() % GRID_SIZE_Y] = true;
    }

    for (int i = 0; i < 20; i++) {
        obstacleGrid[rand() % GRID_SIZE_X][rand() % GRID_SIZE_Y] = true;
    }

    // SFML için kare çizimi
    sf::RectangleShape cleaner(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    cleaner.setFillColor(sf::Color::Green);

    sf::RectangleShape garbage(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    garbage.setFillColor(sf::Color::Red);

    sf::RectangleShape obstacle(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    obstacle.setFillColor(sf::Color::Black);

    sf::Clock clock; // Zamanlayıcı
    std::vector<__UINT8_FAST_LOCATION__> path; // Yol
    size_t pathIndex = 0; // Yolu izlemek için bir indeks
    bool returning = false; // Geri dönüş durumu

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::White);

        // Çöpleri ve engelleri çiz
        for (int x = 0; x < GRID_SIZE_X; x++) {
            for (int y = 0; y < GRID_SIZE_Y; y++) {
                if (garbageGrid[x][y]) {
                    garbage.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                    window.draw(garbage);
                }
                if (obstacleGrid[x][y]) {
                    obstacle.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                    window.draw(obstacle);
                }
            }
        }

        // Temizleyiciyi çiz
        cleaner.setPosition(cleanerLocation.x * CELL_SIZE, cleanerLocation.y * CELL_SIZE);
        window.draw(cleaner);

        // Temizleyiciyi hedefe doğru hareket ettir
        if (pathIndex < path.size() && clock.getElapsedTime().asMilliseconds() >= MOVE_DELAY_MS) {
            // Eğer temizleyici çöpün bulunduğu hücreye gelirse, çöpü temizle
            if (garbageGrid[cleanerLocation.x][cleanerLocation.y]) {
                garbageGrid[cleanerLocation.x][cleanerLocation.y] = false; // Çöpü temizle
            }
            cleanerLocation = path[pathIndex];
            pathIndex++;
            clock.restart(); // Zamanlayıcıyı yeniden başlat
        } 

        // Eğer yol bitti ise, yeni bir hedef belirle
        if (pathIndex >= path.size()) {
            // En yakın çöpü bul
            __UINT8_FAST_LOCATION__ nearestGarbage = FindNearestGarbage(cleanerLocation, garbageGrid);
            if (nearestGarbage.x < GRID_SIZE_X && nearestGarbage.y < GRID_SIZE_Y) { // Geçerli bir çöp bulunduysa
                path = AStar(cleanerLocation, nearestGarbage, obstacleGrid);
                pathIndex = 0; // Yolu yeniden başlat
            } else if (!returning) {
                // Eğer çöp yoksa, geri dönüşe geç
                returning = true; 
                path = AStar(cleanerLocation, startLocation, obstacleGrid); // Başlangıç konumuna dön
                pathIndex = 0; // Yolu yeniden başlat
            }
        }

        // Eğer geri dönüş yapılıyorsa ve yol yoksa, mesaj göster
        if (returning && path.empty()) {
            std::cout << "Temizleyici geri dönüyor ama yol bulamıyor." << std::endl;
            break; // Geri dönmekteyken yol bulamazsa programdan çık
        }

        window.display();
    }

    return 0;
}
