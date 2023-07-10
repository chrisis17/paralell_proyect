#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
#include <chrono>
#include <fstream>

using namespace std;

#define lli long long int

vector<vector<lli>> crearMatrizIncidencia(const string& archivo) {
    ifstream entrada(archivo);
    if (!entrada) {
        cerr << "No se pudo abrir el archivo." << endl;
        return {};
    }

    lli cantidadVertices;
    entrada >> cantidadVertices;

    vector<vector<lli>> matrizIncidencia(cantidadVertices, vector<lli>(cantidadVertices, INT_MAX));

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

class Node
{
public:
    vector<pair<lli, lli>> path;
    vector<vector<lli>> matrix;
    lli cost;
    lli v;
    lli level;

public:
    Node(lli n, const vector<vector<lli>>& matrix, const vector<pair<lli, lli>>& path, lli level, lli a, lli b)
    {
        this->path = path;

        if (level != 0)
        {
            this->path.push_back({a, b});
        }

        this->matrix = matrix;

        for (lli k = 0; level != 0 && k < n; k++)
        {
            this->matrix[a][k] = INT_MAX;
            this->matrix[k][b] = INT_MAX;
        }

        this->matrix[b][0] = INT_MAX;
        this->level = level;
        this->v = b;
    }
};

lli get_min_and_subtract(lli n, vector<vector<lli>>& mat, lli prev_cost)
{
    lli count = 0;

    vector<lli> rc(n);

    for (lli i = 0; i < n; i++)
    {
        lli min = INT_MAX;
        for (lli j = 0; j < n; j++)
        {
            if (min > mat[i][j])
            {
                min = mat[i][j];
            }
        }

        if (min != 0 && min != INT_MAX)
        {
            for (lli j = 0; j < n; j++)
            {
                if (mat[i][j] != INT_MAX)
                {
                    mat[i][j] -= min;
                    count++;
                }
            }
            rc[i] = min;
        }
        else
        {
            rc[i] = 0;
        }
    }

    for (lli j = 0; j < n; j++)
    {
        lli min = INT_MAX;
        for (lli i = 0; i < n; i++)
        {
            if (min > mat[i][j])
            {
                min = mat[i][j];
            }
        }

        if (min != 0 && min != INT_MAX)
        {
            for (lli i = 0; i < n; i++)
            {
                if (mat[i][j] != INT_MAX)
                {
                    mat[i][j] -= min;
                    count++;
                }
            }
            rc[j] += min;
            count++;
        }
    }

    lli cost = 0;
    for (lli i = 0; i < n; i++)
    {
        cost += rc[i];
        count++;
    }
    count++;

    // cout << count << endl;

    return cost + prev_cost;
}

class cmp
{
public:
    bool operator() (Node* a, Node* b)
    {
        return a->cost > b->cost;
    }
};

lli tsp(const vector<vector<lli>>& matrix, lli n)
{
    priority_queue<Node*, vector<Node*>, cmp> pq;
    vector<pair<lli, lli>> p;

    Node* root = new Node(n, matrix, p, 0, -1, 0);

    lli count = 0;

    root->cost = get_min_and_subtract(n, root->matrix, 0);

    pq.push(root);

    while (!pq.empty())
    {
        Node* min = pq.top();
        pq.pop();

        lli i = min->v;

        if (min->level == n - 1)
        {
            min->path.push_back({i, 0});
            return min->cost;
        }

        for (lli j = 0; j < n; j++)
        {
            if (min->matrix[i][j] != INT_MAX)
            {
                Node* child = new Node(n, min->matrix, min->path, min->level + 1, i, j);
                child->cost = min->matrix[i][j] + get_min_and_subtract(n, child->matrix, min->cost);
                count++;
                pq.push(child);
            }
        }
    }

    return -1; // Si no se encuentra una solución válida
}

int main()
{
    // ifstream file("matrix2.txt");

    // lli n;
    // file >> n;
    vector<vector<lli>> matrizIncidencia = crearMatrizIncidencia("map11.tsp");
    // vector<vector<lli>> matrix(n, vector<lli>(n));

    // for (lli i = 0; i < n; i++)
    // {
    //     for (lli j = 0; j < n; j++)
    //     {
    //         lli temp;
    //         file >> temp;
    //         if (temp == -1)
    //         {
    //             matrix[i][j] = INT_MAX;
    //         }
    //         else
    //         {
    //             matrix[i][j] = temp;
    //         }
    //     }
    // }
    lli res = tsp(matrizIncidencia, matrizIncidencia.size());
    cout << "Costo mínimo del recorrido: " << res << endl;

    return 0;
}
