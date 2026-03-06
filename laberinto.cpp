#include <iostream>     //entrada y salida cin cout
#include <vector>       //vector para manejar la grilla del laberinto
#include <stack>        //pila para el DFS
#include <cstdlib>      //para la funcion de aleatoriedad
#include <ctime>        //crea una semilla para la aleatoriedad
#include <chrono>       //para medir el tiempo de ejecución

using namespace std;        //para evitar escribir std:: antes de cada cosa
using namespace chrono;     //para evitar escribir chrono:: antes de cada cosa

struct Celda {               //estructura para representar cada celda del laberinto
    bool visitado;           //
    bool paredes[4]; // 0=arriba,1=derecha,2=abajo,3=izquierda

    Celda() {
        visitado = false;
        for (int i = 0; i < 4; i++)
            paredes[i] = true;
    }
};

int N;
vector<vector<Celda>> casilla;          //grilla del laberinto
vector<vector<bool>> solucion;          //grilla para marcar el camino de la solución      
vector<vector<bool>> caminoSolucion;    //grilla para marcar las celdas visitadas durante la resolución

//--------------------------------------------------
// GENERACIÓN
//--------------------------------------------------

void removerpared(int x1, int y1, int x2, int y2) {         //elimina la pared al pasar de una celda a otra, recibe las coordenadas de ambas celdas

    if (x1 == x2) {
        if (y1 > y2) {
            casilla[x1][y1].paredes[3] = false;
            casilla[x2][y2].paredes[1] = false;
        } else {
            casilla[x1][y1].paredes[1] = false;
            casilla[x2][y2].paredes[3] = false;
        }
    }
    else if (y1 == y2) {
        if (x1 > x2) {
            casilla[x1][y1].paredes[0] = false;
            casilla[x2][y2].paredes[2] = false;
        } else {
            casilla[x1][y1].paredes[2] = false;
            casilla[x2][y2].paredes[0] = false;
        }
    }
}

vector<pair<int,int>> vecinosnovisitados(int x, int y) {        //devuelve una lista de vecinos no visitados de la celda actual

    vector<pair<int,int>> vecinos;                              //vector para almacenar los vecinos no visitados, con coordenadas (x,y)

    if (x > 0 && !casilla[x-1][y].visitado)                     
        vecinos.push_back({x-1, y});

    if (x < N-1 && !casilla[x+1][y].visitado)
        vecinos.push_back({x+1, y});

    if (y > 0 && !casilla[x][y-1].visitado)
        vecinos.push_back({x, y-1});

    if (y < N-1 && !casilla[x][y+1].visitado)
        vecinos.push_back({x, y+1});

    return vecinos;
}

void generarlaberinto() {

    stack<pair<int,int>> s;         //pila para el backtracking, almacena las coordenadas de las celdas visitadas para poder retroceder cuando no haya vecinos disponibles

    int totaldeCasillas = N * N;    //total de celdas en el laberinto, para saber cuándo se ha visitado todas las celdas
    int casillasVisitadas = 1;      //contador de celdas visitadas, inicia en 1 porque se marca la celda inicial como visitada

    int actualX = 0;               //coordenada 'x' de la celda actual
    int actualY = 0;               //coordenada 'y' de la celda actual

    casilla[actualX][actualY].visitado = true;    //marca la celda inicial como visitada

    while (casillasVisitadas < totaldeCasillas) {       //mientras no se hayan visitado todas las celdas, sigue generando el laberinto

        auto vecinos = vecinosnovisitados(actualX, actualY);          //obtiene la lista de vecinos no visitados de la celda actual

        if (!vecinos.empty()) {                                         //si hay vecinos no visitados, elige uno al azar para continuar la generación del laberinto

            int seleccionador = rand() % vecinos.size();                    //genera un índice aleatorio para seleccionar un vecino de la lista
            int nuevaX = vecinos[seleccionador].first;                      //coordenada 'x' del vecino seleccionado
            int nuevaY = vecinos[seleccionador].second;                     //coordenada 'y' del vecino seleccionado

            s.push({actualX, actualY});                     //guarda la celda actual en la pila antes de moverse a la nueva celda, para poder retroceder si es necesario
            removerpared(actualX, actualY, nuevaX, nuevaY); 

            actualX = nuevaX;           //actualiza las coordenadas de la celda actual a la nueva celda seleccionada
            actualY = nuevaY;

            casilla[actualX][actualY].visitado = true;      //marca la nueva celda como visitada
            casillasVisitadas++;                            //incrementa el contador de celdas visitadas
        }
        else if (!s.empty()) {       //si no hay vecinos no visitados pero la pila no está vacía, retrocede a la última celda visitada para buscar otros caminos

            actualX = s.top().first;        //actualiza las coordenadas de la celda actual a la última celda guardada en la pila, para retroceder a ella
            actualY = s.top().second;
            s.pop();                        //elimina la última celda de la pila después de retroceder a ella
        }
    }
}

//--------------------------------------------------
// RESOLUCIÓN DFS
//--------------------------------------------------

bool solucionDFS(int x, int y) {               

    if (x == N-1 && y == N-1) {             //si se llega a la celda final, marca la lista de casillas que llevan a la salida. Se detiene la busqueda
        solucion[x][y] = true;
        return true;
    }

    caminoSolucion[x][y] = true;            //Marca las celdas que recorre durante la busqueda, para evitar ciclos y volver a visitar celdas ya exploradas.
    solucion[x][y] = true;                  //Marca las celdas que llevan a la salida.

    // Arriba
    if (!casilla[x][y].paredes[0] && x > 0 && !caminoSolucion[x-1][y])     //Primero trata de ir hacia arriba 
        if (solucionDFS(x-1, y)) return true;

    // Derecha
    if (!casilla[x][y].paredes[1] && y < N-1 && !caminoSolucion[x][y+1])    //Luego hacia la derecha
        if (solucionDFS(x, y+1)) return true;

    // Abajo
    if (!casilla[x][y].paredes[2] && x < N-1 && !caminoSolucion[x+1][y])    //Luego hacia abajo
        if (solucionDFS(x+1, y)) return true;

    // Izquierda
    if (!casilla[x][y].paredes[3] && y > 0 && !caminoSolucion[x][y-1])      //Finalmente hacia la izquierda
        if (solucionDFS(x, y-1)) return true;

    solucion[x][y] = false;             //Si ninguna de las direcciones lleva a la salida, desmarca la celda actual como parte de la solución y retrocede
    return false;   
}

//--------------------------------------------------
// IMPRESIÓN
//--------------------------------------------------

void imprimirLaberinto() {

    casilla[0][0].paredes[0] = false;           //Asegura que la celda de inicio no tenga pared arriba para que se vea la entrada del laberinto
    casilla[N-1][N-1].paredes[2] = false;       //Asegura que la celda de salida no tenga pared abajo para que se vea la salida del laberinto

    for (int j = 0; j < N; j++) {               //Imprimi el borde superior del laberinto
        cout << "#";
        if (casilla[0][j].paredes[0]) cout << "###";
        else cout << "   ";
    }
    cout << "#\n";                              //Salto de línea después de imprimir el borde superior

    for (int i = 0; i < N; i++) {               //Imprime cada fila del laberinto, incluyendo las paredes verticales y los espacios para las celdas

        for (int j = 0; j < N; j++) {

            if (casilla[i][j].paredes[3]) cout << "#";  //Paredes verticales a la izquierda de cada celda
            else cout << " ";

            if (solucion[i][j]) cout << " * "; 
            else cout << "   ";
        }

        if (casilla[i][N-1].paredes[1]) cout << "#";    //Pared vertical a la derecha de la última celda de cada fila
        else cout << " ";

        cout << "\n";

        for (int j = 0; j < N; j++) {                   //Imprime las paredes horizontales debajo de cada celda
            cout << "#";
            if (casilla[i][j].paredes[2]) cout << "###";
            else cout << "   ";
        }
        cout << "#\n";
    }
}

//--------------------------------------------------

int main() {

    srand(time(0));

    const int TAM_BASE = 10;

    N = TAM_BASE;

    casilla = vector<vector<Celda>>(N, vector<Celda>(N));
    solucion = vector<vector<bool>>(N, vector<bool>(N,false));
    caminoSolucion = vector<vector<bool>>(N, vector<bool>(N,false));

    auto tiempototal = high_resolution_clock::now();

    auto tiempogeneracion = high_resolution_clock::now();
    generarlaberinto();
    auto terminargeneracion = high_resolution_clock::now();

    auto tiemporesolucion = high_resolution_clock::now();
    solucionDFS(0, 0);
    auto terminarsolucion = high_resolution_clock::now();

    auto terminartotal = high_resolution_clock::now();

    imprimirLaberinto();

    cout << "\nTiempo generacion: "
         << duration_cast<microseconds>(terminargeneracion - tiempogeneracion).count()
         << " Us";

    cout << "\nTiempo resolucion: "
         << duration_cast<microseconds>(terminarsolucion - tiemporesolucion).count()
         << " Us";

    cout << "\nTiempo total: "
         << duration_cast<microseconds>(terminartotal - tiempototal).count()
         << " Us\n";

    cout << "Ingrese tamaño del laberinto: ";
    cin >> N;

    casilla = vector<vector<Celda>>(N, vector<Celda>(N));               //Matriz de celdas para representar el laberinto
    solucion = vector<vector<bool>>(N, vector<bool>(N, false));         //Matriz de booleanos para marcar el camino de la solución, inicia con todas las celdas como falsas
    caminoSolucion = vector<vector<bool>>(N, vector<bool>(N, false));   //Matriz de booleanos para marcar las celdas visitadas durante la resolución.

    auto tiempototal2 = high_resolution_clock::now();

    auto tiempogeneracion2 = high_resolution_clock::now();
    generarlaberinto();
    auto terminargeneracion2 = high_resolution_clock::now();

    auto tiemporesolucion2 = high_resolution_clock::now();
    solucionDFS(0, 0);
    auto terminarsolucion2 = high_resolution_clock::now();

    auto terminartotal2 = high_resolution_clock::now();

    imprimirLaberinto();

    cout << "\nTiempo generacion: "
         << duration_cast<microseconds>(terminargeneracion2 - tiempogeneracion2).count()
         << " Us";

    cout << "\nTiempo resolucion: "
         << duration_cast<microseconds>(terminarsolucion2 - tiemporesolucion2).count()
         << " Us";

    cout << "\nTiempo total: "
         << duration_cast<microseconds>(terminartotal2 - tiempototal2).count()
         << " Us\n";

    return 0;
}

