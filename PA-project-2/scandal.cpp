#include <bits/stdc++.h>
using namespace std;

#define MOD ((int)1e9 + 7)
#define NMAX ((int)1e5 + 5)

class TaskScandal {
 public:
    void solve() {
        read_input();
        print_output(get_result());
    }

 private:
    int n, m;
    vector<int> adj[2 * NMAX], rev_adj[2 * NMAX];
    vector<int> components;
    stack<int> S;
    vector<bool> visited;
    int numComponents = 0;

    void read_input() {
        vector<int> aux[NMAX];
        ifstream fin("scandal.in");

        fin >> n >> m;

        for (int i = 0, x, y, c; i < m; ++i) {
            fin >> x >> y >> c;
            switch (c) {
                case 0:
                    add_edge(neg(x), y);
                    add_edge(neg(y), x);
                    break;
                case 1:
                    add_edge(neg(x), neg(y));
                    add_edge(y, x);
                    break;
                case 2:
                    add_edge(neg(y), neg(x));
                    add_edge(x, y);
                    break;
                case 3:
                    add_edge(x, neg(y));
                    add_edge(y, neg(x));
                    break;
            }
        }
        fin.close();
    }

    /* Returneaza indicele nodului complementar */
    int neg(int node) {
        return (node <= n) ? (node + n) : (node - n);
    }

    /* Functie pentru adaugat muchie in graful normal si cel invers */
    void add_edge(int x, int y) {
        adj[x].push_back(y);
        rev_adj[y].push_back(x);
    }

    /* Algoritmul lui Kosaraju */
    void dfs_1(int node) {
        visited[node] = true;
        for (auto &neigh : adj[node])
            if (!visited[neigh])
                dfs_1(neigh);
        S.push(node);
    }

    void dfs_2(int node) {
        visited[node] = true;
        components[node] = numComponents;
        for (auto &neigh : rev_adj[node])
            if (!visited[neigh])
                dfs_2(neigh);
    }

    void Kosaraju() {
        visited.assign(2 * n + 1, false);
        components.assign(2 * n + 1, -1);
        for (int i = 1; i <= 2 * n; ++i) {
            if (!visited[i])
                dfs_1(i);
        }

        visited.assign(2 * n + 1, false);
        while (!S.empty()) {
            int node = S.top();
            S.pop();
            if (!visited[node]) {
                numComponents ++;
                dfs_2(node);
            }
        }
    }

    vector<int> get_result() {
        vector<int> solution;
        Kosaraju();
        /* Construirea solutiei */
        for (int i = 1; i <= n; ++i) {
            if (components[i] > components[neg(i)])
                solution.push_back(i);
        }
        return solution;
    }

    void print_output(vector<int> solution) {
        ofstream fout("scandal.out");
        fout << solution.size() << "\n";
        for (long unsigned int i = 0; i < solution.size(); ++i)
            fout << solution[i] << "\n";
        fout.close();
    }
};

int main() {
    auto* task = new (nothrow) TaskScandal();
    task->solve();
    delete task;
    return 0;
}
