#include <iostream>
#include <vector>
using namespace std;

int main() {
    // 0:A, 1:B, 2:C, 3:D, 4:E
    const int n = 5;
    vector<vector<int>> adj(n, vector<int>(n, 0));

    auto addEdge = [&](int u, int v) {
        adj[u][v] = 1;
        adj[v][u] = 1; // undirected friendship
    };

    // Given friendships:
    addEdge(0, 2); // A-C
    addEdge(1, 3); // B-D
    addEdge(4, 0); // E-A
    addEdge(4, 1); // E-B
    addEdge(4, 2); // E-C
    addEdge(4, 3); // E-D

    // Print adjacency matrix
    cout << "  A B C D E\n";
    for (int i = 0; i < n; i++) {
        cout << char('A' + i) << " ";
        for (int j = 0; j < n; j++) {
            cout << adj[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}
