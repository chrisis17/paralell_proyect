#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <omp.h>

using namespace std;

const int INF = numeric_limits<int>::max() / 2;

struct Edge {
    int node1;
    int node2;
    int weight;
};

vector<vector<int>> readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    int numNodes;
    file >> numNodes;

    vector<vector<int>> matrix(numNodes, vector<int>(numNodes, INF));

    int node1, node2, weight;
    while (file >> node1 >> node2 >> weight) {
        matrix[node1 - 1][node2 - 1] = weight;
        matrix[node2 - 1][node1 - 1] = weight;
    }

    file.close();
    return matrix;
}

vector<int> tsp_omp(const vector<vector<int>>& matrix) {
    int numNodes = matrix.size();
    vector<int> nodes(numNodes - 1);
    vector<int> bestPath;
    int optimalValue = INF;

    #pragma omp parallel
    {
        #pragma omp for nowait
        for (int i = 1; i < numNodes; i++) {
            nodes[i - 1] = i;
        }

        do {
            vector<int> permutation(nodes);
            permutation.insert(permutation.begin(), 0);
            permutation.push_back(0);

            int cost = 0;
            for (int i = 1; i < numNodes; i++) {
                cost += matrix[permutation[i - 1]][permutation[i]];
            }

            #pragma omp critical
            {
                if (cost < optimalValue) {
                    optimalValue = cost;
                    bestPath = permutation;
                }
            }
        } while (next_permutation(nodes.begin(), nodes.end()));
    }

    return bestPath;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./program input_file" << endl;
        return 1;
    }

    string filename = argv[1];
    vector<vector<int>> matrix = readGraphFromFile(filename);

    omp_set_num_threads(omp_get_num_procs());

    vector<int> bestPath;

    double startTime = omp_get_wtime();
    bestPath = tsp_omp(matrix);
    double endTime = omp_get_wtime();

    cout << "Optimal Path: ";
    for (int node : bestPath) {
        cout << node << " ";
    }
    cout << endl;

    // cout << "Optimal Cost: " << optimalValue << endl;

    cout << "Elapsed Time: " << (endTime - startTime) << " seconds" << endl;

    return 0;
}
