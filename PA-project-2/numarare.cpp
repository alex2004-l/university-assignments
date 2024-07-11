#include <bits/stdc++.h>
using namespace std;

#define MOD ((int)1e9 + 7)
#define NMAX ((int)1e5 + 1)

class TaskNumarare {
 public:
    void solve() {
        read_input();
        print_output(get_result());
    }

 private:
    int n, m;
    vector<int> adj[NMAX];
    int in_degree[NMAX] = {0};

    /* Se construieste un graf cu muchiile comune */
    void read_input() {
        vector<int> aux[NMAX];
        ifstream fin("numarare.in");
        fin >> n >> m;
        for (int i = 1, x, y; i <= m; ++i) {
            fin >> x >> y;
            aux[x].push_back(y);
        }
        for (int i = 1, x, y; i <= m; ++i) {
            fin >> x >> y;
            if (find(aux[x].begin(), aux[x].end(), y) != aux[x].end()) {
                adj[x].push_back(y);
                in_degree[y] ++;
            }
        }

        fin.close();
    }

    int get_result() {
        /* Se aplica algoritmul de sortare topologica */
        queue<int> q;

        for (int i = 1; i <= n; ++i) {
            if (!in_degree[i]) {
                q.push(i);
            }
        }

        vector<int> topological_order;
        while (!q.empty()) {
            int node = q.front();
            q.pop();

            topological_order.push_back(node);

            for (auto it = adj[node].begin(); it != adj[node].end(); ++it) {
                --in_degree[*it];
                if (!in_degree[*it]) {
                    q.push(*it);
                }
            }
        }

        /* Se calculeaza in sens invers numarul drumurilor elementare */
        /* Am considerat numarul drumurilor ca un cost asociat fiecarui nod */
        int size = topological_order.size();
        vector<int> costs(n + 1, 0);

        costs[n] = 1;

        for (int i = size - 1; i >= 0; --i) {
            int node = topological_order[i];
            for (auto it = adj[node].begin(); it != adj[node].end(); ++it) {
                costs[node] += costs[*it];
                costs[node] = costs[node] % MOD;
            }
        }

        return costs[1];
    }

    void print_output(int result) {
        ofstream fout("numarare.out");
        fout << result << "\n";
        fout.close();
    }
};

int main() {
    auto* task = new (nothrow) TaskNumarare();
    task->solve();
    delete task;
    return 0;
}
