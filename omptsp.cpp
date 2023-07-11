#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <omp.h>

using namespace std;
int num_threads = 1;
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
            if (valor == INT_MAX)
                cout << -1 << " ";
            else
                cout << valor << " ";
        }
        cout << endl;
    }

    return matrizIncidencia;
}

int firstMin(vector<vector<int>>& adj, int i, int& N) {
    int min_val = INT_MAX;
    for (int k = 0; k < N; k++)
        if (adj[i][k] < min_val && i != k)
            min_val = adj[i][k];
    return min_val;
}

int secondMin(vector<vector<int>>& adj, int i, int& N) {
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

void copyToFinal(vector<int>& curr_path, int& N, vector<int>& final_path) {
    for (int i = 0; i < N; i++)
        final_path[i] = curr_path[i];
    final_path[N] = curr_path[0];
}

void TSPRec(vector<vector<int>>& adj, int curr_bound, int curr_weight, int level, vector<int>& curr_path, vector<bool>& visited, vector<int>& final_path, int& N, int& final_res) {
    if (level == N) {
        if (adj[curr_path[level - 1]][curr_path[0]] != 0) {
            int curr_res = curr_weight + adj[curr_path[level - 1]][curr_path[0]];
            if (curr_res < final_res) {
                copyToFinal(curr_path, N, final_path);
                final_res = curr_res;
            }
        }
        return;
    }

    #pragma omp parallel for shared(adj, curr_path, visited) num_threads(num_threads)
    for (int i = 0; i < N; i++) {
        if (adj[curr_path[level - 1]][i] != 0 && visited[i] == false) {
            int temp = curr_bound;
            int local_curr_weight = curr_weight;
            vector<int> local_curr_path(curr_path);
            vector<bool> local_visited(visited);

            local_curr_weight += adj[curr_path[level - 1]][i];

            if (level == 1)
                temp -= ((firstMin(adj, curr_path[level - 1], N) + firstMin(adj, i, N)) / 2);
            else
                temp -= ((secondMin(adj, curr_path[level - 1], N) + firstMin(adj, i, N)) / 2);

            if (temp + local_curr_weight < final_res) {
                local_curr_path[level] = i;
                local_visited[i] = true;
                TSPRec(adj, temp, local_curr_weight, level + 1, local_curr_path, local_visited, final_path, N, final_res);
            }
        }
    }
}

void TSP(vector<vector<int>>& adj, vector<bool>& visited, vector<int>& final_path, int& N, int& final_res) {
    vector<int> curr_path(N + 1);
    int curr_bound = 0;
    curr_path[0] = 0;

    for (int i = 0; i < N; i++)
        curr_bound += (firstMin(adj, i, N) + secondMin(adj, i, N));

    curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;

    visited[0] = true;
    TSPRec(adj, curr_bound, 0, 1, curr_path, visited, final_path, N, final_res);
}

int main() {
    vector<vector<int>> matrizIncidencia = crearMatrizIncidencia("e/11nodes.tsp");
    int N = matrizIncidencia.size();
    vector<int> final_path(N + 1);

    vector<bool> visited(N, false);
    int final_res = INT_MAX;

    double start_time = omp_get_wtime();
    TSP(matrizIncidencia, visited, final_path, N, final_res);
    double end_time = omp_get_wtime();

    cout << "Costo mínimo: " << final_res << endl;
    cout << "Camino tomado: ";
    for (int i = 0; i <= N; i++)
        cout << final_path[i] << " ";

    cout << endl << "Tiempo de ejecución: " << (end_time - start_time) << " segundos." << endl;

    return 0;
}
