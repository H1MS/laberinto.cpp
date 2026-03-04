#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

struct Cell {
    bool visited;
    bool walls[4]; // 0=arriba,1=derecha,2=abajo,3=izquierda

    Cell() {
        visited = false;
        for (int i = 0; i < 4; i++)
            walls[i] = true;
    }
};

int N;
vector<vector<Cell>> grid;
vector<vector<bool>> solution;
vector<vector<bool>> visitedSolve;

//--------------------------------------------------
// GENERACIÓN
//--------------------------------------------------

void removeWalls(int x1, int y1, int x2, int y2) {

    if (x1 == x2) {
        if (y1 > y2) {
            grid[x1][y1].walls[3] = false;
            grid[x2][y2].walls[1] = false;
        } else {
            grid[x1][y1].walls[1] = false;
            grid[x2][y2].walls[3] = false;
        }
    }
    else if (y1 == y2) {
        if (x1 > x2) {
            grid[x1][y1].walls[0] = false;
            grid[x2][y2].walls[2] = false;
        } else {
            grid[x1][y1].walls[2] = false;
            grid[x2][y2].walls[0] = false;
        }
    }
}

vector<pair<int,int>> getUnvisitedNeighbors(int x, int y) {

    vector<pair<int,int>> neighbors;

    if (x > 0 && !grid[x-1][y].visited)
        neighbors.push_back({x-1, y});

    if (x < N-1 && !grid[x+1][y].visited)
        neighbors.push_back({x+1, y});

    if (y > 0 && !grid[x][y-1].visited)
        neighbors.push_back({x, y-1});

    if (y < N-1 && !grid[x][y+1].visited)
        neighbors.push_back({x, y+1});

    return neighbors;
}

void generateMaze() {

    stack<pair<int,int>> s;

    int totalCells = N * N;
    int visitedCells = 1;

    int currentX = 0;
    int currentY = 0;

    grid[currentX][currentY].visited = true;

    while (visitedCells < totalCells) {

        auto neighbors = getUnvisitedNeighbors(currentX, currentY);

        if (!neighbors.empty()) {

            int randIndex = rand() % neighbors.size();
            int nextX = neighbors[randIndex].first;
            int nextY = neighbors[randIndex].second;

            s.push({currentX, currentY});
            removeWalls(currentX, currentY, nextX, nextY);

            currentX = nextX;
            currentY = nextY;

            grid[currentX][currentY].visited = true;
            visitedCells++;
        }
        else if (!s.empty()) {

            currentX = s.top().first;
            currentY = s.top().second;
            s.pop();
        }
    }
}

//--------------------------------------------------
// RESOLUCIÓN DFS
//--------------------------------------------------

bool solveDFS(int x, int y) {

    if (x == N-1 && y == N-1) {
        solution[x][y] = true;
        return true;
    }

    visitedSolve[x][y] = true;
    solution[x][y] = true;

    // Arriba
    if (!grid[x][y].walls[0] && x > 0 && !visitedSolve[x-1][y])
        if (solveDFS(x-1, y)) return true;

    // Derecha
    if (!grid[x][y].walls[1] && y < N-1 && !visitedSolve[x][y+1])
        if (solveDFS(x, y+1)) return true;

    // Abajo
    if (!grid[x][y].walls[2] && x < N-1 && !visitedSolve[x+1][y])
        if (solveDFS(x+1, y)) return true;

    // Izquierda
    if (!grid[x][y].walls[3] && y > 0 && !visitedSolve[x][y-1])
        if (solveDFS(x, y-1)) return true;

    solution[x][y] = false;
    return false;
}

//--------------------------------------------------
// IMPRESIÓN
//--------------------------------------------------

void printMaze() {

    grid[0][0].walls[0] = false;
    grid[N-1][N-1].walls[2] = false;

    for (int j = 0; j < N; j++) {
        cout << "+";
        if (grid[0][j].walls[0]) cout << "---";
        else cout << "   ";
    }
    cout << "+\n";

    for (int i = 0; i < N; i++) {

        for (int j = 0; j < N; j++) {

            if (grid[i][j].walls[3]) cout << "|";
            else cout << " ";

            if (solution[i][j]) cout << " * ";
            else cout << "   ";
        }

        if (grid[i][N-1].walls[1]) cout << "|";
        else cout << " ";

        cout << "\n";

        for (int j = 0; j < N; j++) {
            cout << "+";
            if (grid[i][j].walls[2]) cout << "---";
            else cout << "   ";
        }
        cout << "+\n";
    }
}

//--------------------------------------------------

int main() {

    srand(time(0));

    cout << "Ingrese tamaño del laberinto: ";
    cin >> N;

    grid = vector<vector<Cell>>(N, vector<Cell>(N));
    solution = vector<vector<bool>>(N, vector<bool>(N, false));
    visitedSolve = vector<vector<bool>>(N, vector<bool>(N, false));

    auto startTotal = high_resolution_clock::now();

    auto startGen = high_resolution_clock::now();
    generateMaze();
    auto endGen = high_resolution_clock::now();

    auto startSolve = high_resolution_clock::now();
    solveDFS(0, 0);
    auto endSolve = high_resolution_clock::now();

    auto endTotal = high_resolution_clock::now();

    printMaze();

    cout << "\nTiempo generacion: "
         << duration_cast<milliseconds>(endGen - startGen).count()
         << " ms";

    cout << "\nTiempo resolucion: "
         << duration_cast<milliseconds>(endSolve - startSolve).count()
         << " ms";

    cout << "\nTiempo total: "
         << duration_cast<milliseconds>(endTotal - startTotal).count()
         << " ms\n";

    return 0;
}