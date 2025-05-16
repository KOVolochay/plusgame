#include<bits/stdc++.h>

using namespace std;

#define COLOR_RESET   "\033[0m"
#define COLOR_PLAYER  "\033[1;32m" 
#define COLOR_ENEMY   "\033[1;31m"
#define COLOR_APPLE   "\033[1;33m"

const int WIDTH = 30;
const int HEIGHT = 15;
const char WALL = '#';
const char SPACE = ' ';
const char PLAYER = 'P';
const char ENEMY = 'E';
const char APPLE = 'A';

struct Point {
    int x, y;
    bool operator==(const Point& other) const 
    {
        return x == other.x && y == other.y;
    }
    bool operator<(const Point& other) const 
    {

        return tie(x, y) < tie(other.x, other.y);
    }
    bool operator!=(const Point &other) const
    {
        return x != other.x || y != other.y;
    }
};

vector<vector<char> > maze;
Point player, enemy, apple = {-1, -1};
int col = 0;
int steps = 0;

bool free(int x, int y)
{
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && maze[y][x] == SPACE;
}

void generateMaze() {
    const int ROOM_SIZE = 5;
    const int ROOMS_X = WIDTH / ROOM_SIZE;
    const int ROOMS_Y = HEIGHT / ROOM_SIZE;
    srand(time(0));
    vector<vector<string>> roomTemplates = {
        {
            "### #",
            "    #",
            "# # #",
            "#    ",
            "# ###"
        },
        {
            "#### ",
            "#    ",
            "## ##",
            "    #",
            " ####"
        },
        {
            "#  ##",
            "# #  ",
            "#   #",
            "  # #",
            "# ###"
        },
        {
            "##  #",
            "  # #",
            "#  ##",
            "#   #",
            "# # #"
        }
    };

    while (true) {
        maze = vector<vector<char>>(HEIGHT, vector<char>(WIDTH, WALL));
        for (int ry = 0; ry < ROOMS_Y; ++ry) {
            for (int rx = 0; rx < ROOMS_X; ++rx) {
                int offsetY = ry * ROOM_SIZE;
                int offsetX = rx * ROOM_SIZE;

                const auto& room = roomTemplates[rand() % roomTemplates.size()];
                for (int y = 0; y < ROOM_SIZE; ++y) {
                    for (int x = 0; x < ROOM_SIZE; ++x) {
                        maze[offsetY + y][offsetX + x] = room[y][x];
                    }
                }
            }
        }
        for (int ry = 0; ry < ROOMS_Y; ++ry) {
            for (int rx = 0; rx < ROOMS_X - 1; ++rx) {
                int x = (rx + 1) * ROOM_SIZE;
                int y = ry * ROOM_SIZE + ROOM_SIZE / 2;
                maze[y][x - 1] = SPACE;
                maze[y][x] = SPACE;
            }
        }

        for (int ry = 0; ry < ROOMS_Y - 1; ++ry) {
            for (int rx = 0; rx < ROOMS_X; ++rx) {
                int y = (ry + 1) * ROOM_SIZE;
                int x = rx * ROOM_SIZE + ROOM_SIZE / 2;
                maze[y - 1][x] = SPACE;
                maze[y][x] = SPACE;
            }
        }
        vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH, false));
        Point start = {1, 1};

        if (!free(start.x, start.y)) continue;

        queue<Point> q;
        q.push(start);
        visited[start.y][start.x] = true;

        while (!q.empty()) {
            Point p = q.front(); q.pop();
            for (auto [dx, dy] : vector<Point>{{1,0},{-1,0},{0,1},{0,-1}}) {
                int nx = p.x + dx, ny = p.y + dy;
                if (free(nx, ny) && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    q.push({nx, ny});
                }
            }
        }

        bool allReachable = true;
        for (int y = 0; y < HEIGHT; ++y)
            for (int x = 0; x < WIDTH; ++x)
                if (maze[y][x] == SPACE && !visited[y][x])
                    allReachable = false;

        if (allReachable)
            break;
    }
}


void drawMaze() {
    system("clear");
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (player.x == x && player.y == y)
                cout << COLOR_PLAYER << PLAYER << COLOR_RESET;
            else if (enemy.x == x && enemy.y == y)
                cout << COLOR_ENEMY << ENEMY << COLOR_RESET;
            else if (apple.x == x && apple.y == y)
                cout << COLOR_APPLE << APPLE << COLOR_RESET;
            else
                cout << maze[y][x];
        }
        cout << '\n';
    }
    cout << "Apples collected: " << col << " / 5\n";
    cout << "Move with WASD : ";
}


vector<Point> bfsPath(Point start, Point goal){
    queue<Point> q;
    map<Point, Point> parent;
    vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH, false));

    q.push(start);
    visited[start.y][start.x] = true;

    while (!q.empty()){
        Point current = q.front(); q.pop();
        if (current == goal) break;

        for (auto [dx, dy] : vector<Point>{{1,0},{-1,0},{0,1},{0,-1}}){
            int nx = current.x + dx, ny = current.y + dy;
            if (free(nx, ny) && !visited[ny][nx]){
                visited[ny][nx] = true;
                Point next = {nx, ny};
                q.push(next);
                parent[next] = current;
            }
        }
    }

    vector<Point> path;
    if (!parent.count(goal)) return {}; 

    Point current = goal;
    while (current != start){
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    return path;
}

void moveEnemy(){
    auto path = bfsPath(enemy, player);
    if (!path.empty())
        enemy = path[0];
}

Point spawn(){
    for (int i = 0; i < 100; ++i){
        int x = rand() % WIDTH, y = rand() % HEIGHT;
        if (free(x, y) && !(x == player.x && y == player.y) && !(x == enemy.x && y == enemy.y)){
            return {x, y};
        }
    }
}

void gameLoop(){
    player = spawn();
    enemy = spawn();
    apple = spawn();
    while (true)
    {
        drawMaze();
        if (player == enemy){
            cout << "You were caught! Game Over.\n";
            break;
        }

        if (col >= 5){
            cout << "You win! Collected all apples!\n";
            break;
        }
        char move;
        cin >> move;
        int dx = 0, dy = 0;
        if (move == 'w' || move == 'W')
            dy = -1;
        else if (move == 's' || move == 'S') 
            dy = 1;
        else if (move == 'a' || move == 'A') 
            dx = -1;
        else if (move == 'd' || move == 'D') 
            dx = 1;
        int nx = player.x + dx;
        int ny = player.y + dy;
        if (free(nx, ny)){
            player = {nx, ny};
            steps++;
        }

        if (player == apple){
            col++;
            apple = {-1, -1};
        }

        if (steps % 10 == 0 && apple.x == -1)
            apple = spawn();

        moveEnemy();
    }
}

int main(){
    generateMaze();
    gameLoop();
}
