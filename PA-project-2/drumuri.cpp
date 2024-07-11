#include <bits/stdc++.h>
#include <functional>
#include <climits>
using namespace std;

#define NMAX ((int)1e5 + 1)

class Compare {
 public:
    bool operator()(pair<int, long long> e1, pair<int, long long> e2) {
        if (e1.second > e2.second) {
            return true;
        }
        return false;
    }
};

class TaskDrumuri {
 public:
    void solve() {
        read_input();
        print_output(get_result());
    }

 private:
    int n, m;
    vector<pair<int, int>> adj[NMAX];
    vector<pair<int, int>> adj2[NMAX];
    int x, y, z;

    void read_input() {
        ifstream fin("drumuri.in");

        fin >> n >> m;

        /* Se construiesc atat graful normal, cat si cel inversat */
        for (int i = 1, a, b, c; i <= m; ++i) {
            fin >> a >> b >> c;
            adj[b].push_back({a, c});
            adj2[a].push_back({b, c});
        }

        fin >> x >> y >> z;

        fin.close();
    }

    /* Functie inspirata din solutia de laborator */
    vector<long long> dijkstra(int source, int nodes,
            vector<pair<int, int>> adj[NMAX]) {
        vector<long long> d(nodes + 1, -1);
        vector<bool> visited(nodes + 1, false);

        priority_queue<pair<int, long long>,
            vector<pair<int, long long>>, Compare> pq;

        d[source] = 0;
        pq.push({source, d[source]});

        while (!pq.empty()) {
            int node = pq.top().first;
            pq.pop();

            if (!visited[node]) {
                visited[node] = true;
                for (const auto& [neigh, w] : adj[node]) {
                    if (d[neigh] == -1 || d[node] + w < d[neigh]) {
                        d[neigh] = d[node] + w;

                        pq.push({neigh, d[neigh]});
                    }
                }
            }
        }

        return d;
    }

    unsigned long long get_result() {
        /* Vectori de distante pornid din x, y si z */
        vector<long long> resultZ = dijkstra(z, n, adj);
        vector<long long> resultX = dijkstra(x, n, adj2);
        vector<long long> resultY = dijkstra(y, n, adj2);

        long long sum = LLONG_MAX;

        /* Calculul sumei minime */
        for (int i = 1; i <= n; ++i) {
            if (resultZ[i] != -1 && resultX[i] != -1 && resultY[i] != -1)
            if (resultZ[i] + resultX[i] + resultY[i] < sum)
                sum = resultZ[i] + resultX[i] + resultY[i];
        }

        return sum;
    }

    void print_output(unsigned long long res) {
        ofstream fout("drumuri.out");
        fout << res << endl;
        fout.close();
    }
};

int main() {
    auto* task = new (nothrow) TaskDrumuri();
    task->solve();
    delete task;
    return 0;
}
