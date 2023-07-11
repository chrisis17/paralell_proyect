#include <bits/stdc++.h>
using namespace std;




vector<vector<int>> crearMatrizIncidencia(const string& archivo) {
    ifstream entrada(archivo);
    if (!entrada) {
        cerr << "No se pudo abrir el archivo." << endl;
        return {};
    }

    int cantidadVertices;
    entrada >> cantidadVertices;

    vector<vector<int>> matrizIncidencia(cantidadVertices, vector<int>(cantidadVertices, INT_MAX));

    int nodoA, nodoB, peso;
    while (entrada >> nodoA >> nodoB >> peso) {
        matrizIncidencia[nodoA][nodoB] = peso;
        matrizIncidencia[nodoB][nodoA] = peso;
    }

    entrada.close();

    for (const auto& fila : matrizIncidencia) {
        for (int valor : fila) {
            if(valor == INT_MAX) cout << -1 << " ";
            else cout << valor << " ";
        }
        cout << endl;
    }

    return matrizIncidencia;
}

// Función para copiar la solución temporal a la solución final
void copyToFinal(vector<int>& curr_path,int &N,vector<int>& final_path) {
    for (int i = 0; i < N; i++)
        final_path[i] = curr_path[i];
    final_path[N] = curr_path[0];
}

// Función para encontrar el mínimo costo de la primera arista que tiene un extremo en el vértice i
int firstMin(vector<vector<int>>& adj, int i,int &N) {
    int min_val = INT_MAX;
    for (int k = 0; k < N; k++)
        if (adj[i][k] < min_val && i != k)
            min_val = adj[i][k];
    return min_val;
}

// Función para encontrar el segundo mínimo costo de la arista que tiene un extremo en el vértice i
int secondMin(vector<vector<int>>& adj, int i, int &N) {
    int first = INT_MAX, second = INT_MAX;
    for (int j = 0; j < N; j++) {
        if (i == j)
            continue;
        if (adj[i][j] <= first) {
            second = first;
            first = adj[i][j];
        } else if (adj[i][j] <= second && adj[i][j] != first)
            second = adj[i][j];
    }
    return second;
}

// Función recursiva para resolver el problema del vendedor viajero
void TSPRec(vector<vector<int>>& adj, int curr_bound, int curr_weight, int level, vector<int>& curr_path, vector<bool>& visited,vector<int>& final_path,int &N,int &final_res) {
    // Caso base: cuando llegamos al nivel N, hemos cubierto todos los nodos una vez
    if (level == N) {
        // Verificamos si hay una arista desde el último vértice en el camino de vuelta al primer vértice
        if (adj[curr_path[level - 1]][curr_path[0]] != 0) {
            int curr_res = curr_weight + adj[curr_path[level - 1]][curr_path[0]];
            // Actualizamos el resultado final y el camino final si el resultado actual es mejor
            if (curr_res < final_res) {
                copyToFinal(curr_path,N,final_path);
                final_res = curr_res;
            }
        }
        return;
    }

    // Para cualquier otro nivel, iteramos sobre todos los vértices para construir el árbol de búsqueda recursivamente
    for (int i = 0; i < N; i++) {
        // Consideramos el siguiente vértice si no es el mismo (entrada diagonal en la matriz de adyacencia) y no ha sido visitado
        if (adj[curr_path[level - 1]][i] != 0 && visited[i] == false) {
            int temp = curr_bound;
            curr_weight += adj[curr_path[level - 1]][i];

            if (level == 1)
                curr_bound -= ((firstMin(adj, curr_path[level - 1],N) + firstMin(adj, i,N)) / 2);
            else
                curr_bound -= ((secondMin(adj, curr_path[level - 1],N) + firstMin(adj, i,N)) / 2);

            // Si el límite actual + peso actual es menor que el resultado final, necesitamos explorar más el nodo
            if (curr_bound + curr_weight < final_res) {
                curr_path[level] = i;
                visited[i] = true;
                TSPRec(adj, curr_bound, curr_weight, level + 1, curr_path,visited,final_path,N,final_res);
            }

            curr_weight -= adj[curr_path[level - 1]][i];
            curr_bound = temp;

            visited.assign(N, false);
            for (int j = 0; j <= level - 1; j++)
                visited[curr_path[j]] = true;
        }
    }
}

// Función principal para resolver el problema del vendedor viajero
void TSP(vector<vector<int>>& adj, vector<bool>& visited,vector<int>& final_path,int &N,int &final_res) {
    vector<int> curr_path(N + 1);
    int curr_bound = 0;
    curr_path[0] = 0;

    // Calculamos el límite inferior inicial para el nodo raíz
    for (int i = 0; i < N; i++)
        curr_bound += (firstMin(adj, i,N) + secondMin(adj, i,N));

    curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;

    visited[0] = true;
    TSPRec(adj, curr_bound, 0, 1, curr_path,visited,final_path,N,final_res);
}

int main() {
    
    vector<vector<int>> matrizIncidencia = crearMatrizIncidencia("map11.tsp");
    int N = matrizIncidencia.size();
    vector<int> final_path(N + 1);

    vector<bool> visited(N, false);
    int final_res = INT_MAX;

    TSP(matrizIncidencia,visited,final_path,N,final_res);

    printf("Costo mínimo: %d\n", final_res);
    printf("Camino tomado: ");
    for (int i = 0; i <= N; i++)
        printf("%d ", final_path[i]);

    return 0;
}
