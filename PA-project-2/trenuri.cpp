#include <bits/stdc++.h>
using namespace std;

#define NMAX ((int)1e5 + 1)

class TaskTrenuri {
 public:
    void solve() {
        read_input();
        print_output(get_result());
    }

 private:
    string start, stop;
    vector<int> adj[NMAX];
    unordered_map<string, int> idx;
    int m, n = 2;
    int in_degree[NMAX] = {0};

    void read_input() {
        ifstream fin("trenuri.in");

        string city1, city2;

        fin >> start >> stop;
        idx.insert({start, 1});
        idx.insert({stop, 2});

        fin >> m;

        /* Daca nu exista un id asociat orasului, se adauga in map
         * cu urmatorul id disponibil */
        for (int i = 1; i <= m; ++i) {
            fin >> city1 >> city2;
            if (idx.find(city1) == idx.end()) {
                ++n;
                idx.insert({city1, n});
            }

            if (idx.find(city2) == idx.end()) {
                ++n;
                idx.insert({city2, n});
            }

            adj[idx.at(city1)].push_back(idx.at(city2));
            in_degree[idx.at(city2)] ++;
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

        /* Se calculeaza in sens invers numarul maxim de noduri prin care *
         * trece un drum de la un nod oarecare la nodul de stop */
        /* Am considerat numarul nodurilor ca un cost asociat fiecarui nod */
        int size = topological_order.size();
        vector<int> costs(n + 1, 0);

        for (int i = size - 1; i >= 0; --i) {
            int node = topological_order[i];
            for (auto it = adj[node].begin(); it != adj[node].end(); ++it) {
                if (costs[*it] > costs[node])
                    costs[node] = costs[*it];
            }
            costs[node] ++;
        }

        return costs[1];
    }

    void print_output(int result) {
        ofstream fout("trenuri.out");
        fout << result << "\n";
        fout.close();
    }
};

int main() {
    auto* task = new (nothrow) TaskTrenuri();
    task->solve();
    delete task;
    return 0;
}
