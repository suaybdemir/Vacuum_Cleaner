#include <iostream>
#include <cstdint>
#include <thread> 
#include <chrono> 
#include <cstdlib> 
#include <ctime> 

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

void Move(Item &cleaner, __UINT8_FAST_AXIS__ &axis, uint_fast8_t pixelNum)
{
    for (int bound = 0; bound < pixelNum; bound++)
    {
        int nextX = cleaner.location.x;
        int nextY = cleaner.location.y;

        if (axis.moveX)
        {
            if (axis.movePositiveX && nextX < GRID_SIZE_X - 1)
            {
                nextX++;
            }
            else if (!axis.movePositiveX && nextX > 0)
            {
                nextX--; 
            }
        }

        if (axis.moveY)
        {
            if (axis.movePositiveY && nextY < GRID_SIZE_Y - 1)
            {
                nextY++;
            }
            else if (!axis.movePositiveY && nextY > 0)
            {
                nextY--;
            }
        }

        if (nextX < GRID_SIZE_X && nextY < GRID_SIZE_Y && !obstacleGrid[nextX][nextY])
        {
            cleaner.location.x = nextX;
            cleaner.location.y = nextY;
        }
        else
        {
            std::cout << "Cleaner is blocked by obstacles.\n";
        }

        PrintGrid(cleaner);
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); 
    }
}

void MoveBackToOrigin(Item &cleaner)
{
    while (cleaner.location.x > 0 || cleaner.location.y > 0)
    {
        if (cleaner.location.x > 0)
        {
            --cleaner.location.x;
        }
        if (cleaner.location.y > 0)
        {
            --cleaner.location.y;
        }

        PrintGrid(cleaner);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

int main()
{
    Item* cleaner = Item::createItem(); 
    __UINT8_FAST_AXIS__ axis; 

    int numGarbage = 10; 
    int numObstacles = 5; 
    PlaceGarbage(numGarbage);
    PlaceObstacle(numObstacles);

    axis.moveX = true;
    axis.movePositiveX = true;
    axis.moveY = false; 
    Move(*cleaner, axis, 10);

    axis.moveX = false; 
    axis.moveY = true;
    axis.movePositiveY = true;
    Move(*cleaner, axis, 12);

    axis.moveX = true;
    axis.movePositiveX = false; 
    axis.moveY = true;
    axis.movePositiveY = false; 
    Move(*cleaner, axis, 10);

    MoveBackToOrigin(*cleaner);

    cout << "\nReturned to the Beginning -> X: " << static_cast<int>(cleaner->location.x) << " Y: " << static_cast<int>(cleaner->location.y) << endl;

    delete cleaner; 
    return 0;
}
