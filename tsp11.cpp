#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <queue>
#include <omp.h>

using namespace std;

const int INF = numeric_limits<int>::max();

// Estructura para representar una arista
struct Edge {
    int src, dest, weight;
};

// Función para leer los datos del archivo
vector<Edge> readData(const string& filename) {
    ifstream file(filename);
    vector<Edge> edges;

    if (file.is_open()) {
        string name;
        int numEdges;
        file >> name >> numEdges;

        int src, dest, weight;
        while (file >> src >> dest >> weight) {
            edges.push_back({src, dest, weight});
        }

        file.close();
    } else {
        cout << "No se pudo abrir el archivo: " << filename << endl;
    }

    return edges;
}

// Función para calcular la distancia entre dos nodos
int distance(const Edge& edge) {
    return edge.weight;
}

// Estructura para representar una tarea
struct Task {
    vector<int> currPath;
    vector<bool> visited;
    int currCost;

    Task() = default; // Constructor sin argumentos

    Task(const vector<int>& path, const vector<bool>& vis, int cost)
        : currPath(path), visited(vis), currCost(cost) {}
};

// Función auxiliar para ordenar las tareas según su costo actual
struct CompareTasks {
    bool operator()(const Task& task1, const Task& task2) const {
        return task1.currCost > task2.currCost;
    }
};

// Función principal del algoritmo de Branch and Bound
void tspBranchAndBound(vector<Edge>& edges, int numThreads, int& minCost, vector<int>& minPath) {
    int n = edges.size() + 1; // Número de nodos

    // Crear cola de tareas
    priority_queue<Task, vector<Task>, CompareTasks> tasks;

    // Crear tarea inicial
    vector<int> initialPath{1};
    vector<bool> initialVisited(n, false);
    initialVisited[1] = true;
    Task initialTask(initialPath, initialVisited, 0);
    tasks.push(initialTask);

    // Calcular el camino óptimo utilizando Branch and Bound
    while (!tasks.empty()) {
        vector<Task> newTasks;

        #pragma omp parallel num_threads(numThreads)
        {
            vector<Task> privateTasks;

            #pragma omp for nowait
            for (int t = 0; t < tasks.size(); t++) {
                Task task = tasks.top();
                tasks.pop();

                if (task.currPath.size() == n) { // Se ha completado un ciclo
                    task.currCost += distance(edges[task.currPath.back() - 1]);
                    if (task.currCost < minCost) {
                        #pragma omp critical
                        {
                            if (task.currCost < minCost) {
                                minCost = task.currCost;
                                minPath = task.currPath;
                            }
                        }
                    }
                } else {
                    for (int i = 1; i <= n; i++) {
                        if (!task.visited[i]) {
                            vector<int> newPath = task.currPath;
                            newPath.push_back(i);

                            vector<bool> newVisited = task.visited;
                            newVisited[i] = true;

                            int edgeIndex = newPath.size() - 2;
                            int edgeWeight = (edgeIndex >= 0) ? distance(edges[newPath[edgeIndex] - 1]) : 0;
                            int newCost = task.currCost + edgeWeight + distance(edges[i - 1]);

                            if (newCost < minCost) {
                                privateTasks.emplace_back(newPath, newVisited, newCost);
                            }
                        }
                    }
                }
            }

            #pragma omp critical
            {
                newTasks.insert(newTasks.end(), privateTasks.begin(), privateTasks.end());
            }
        }

        for (const auto& task : newTasks) {
            tasks.push(task);
        }
    }
}

int main() {
    string filename = "map10.tsp"; // Nombre del archivo con los datos
    int numThreads = omp_get_max_threads(); // Cantidad de hilos
    std::cout << "parte 1" << std::endl;

    // Leer los datos del archivo
    vector<Edge> edges = readData(filename);
    std::cout << "parte 1" << std::endl;

    int n = edges.size() + 1; // Número de nodos
    std::cout << n << std::endl;

    // Inicializar variables
    int minCost = INF;
    vector<int> minPath;
    std::cout << "parte 1" << std::endl;

    // Calcular el camino óptimo utilizando Branch and Bound
    tspBranchAndBound(edges, numThreads, minCost, minPath);
    std::cout << "parte 1" << std::endl;

    // Mostrar el resultado
    cout << "Tiempo de ejecución: " << omp_get_wtime() << " segundos" << endl;
    cout << "Camino óptimo: ";
    for (int node : minPath) {
        cout << node << " ";
    }
    cout << endl;

    return 0;
}
