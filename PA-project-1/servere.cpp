#include <bits/stdc++.h>
using namespace std;

struct Server {
    int power;
    int current;
};

bool compare_servers(Server server1, Server server2) {
    if (server1.power == server2.power) {
        return (server1.current > server2.current);
    }
    return (server1.power < server2.power);
}

class TaskServere {
 public:
    void solve() {
        read_input();
        print_output(get_solution());
    }

 private:
    int servers_number;
    vector<Server> servers;

    void read_input() {
        ifstream fin("servere.in");
        fin >> servers_number;

        vector<int> power;
        for (int i = 0, aux; i < servers_number; ++i) {
            fin >> aux;
            power.push_back(aux);
        }

        vector<int> current;
        for (int i = 0, aux; i < servers_number; ++i) {
            fin >> aux;
            current.push_back(aux);
        }

        Server aux;
        for (int i = 0; i < servers_number; ++i) {
            aux.power = power[i];
            aux.current = current[i];
            servers.push_back(aux);
        }

        fin.close();
    }

    double get_solution() {
        sort(servers.begin(), servers.end(), compare_servers);

        double min_power, power, min_current;
        min_power = servers[0].power;
        min_current = servers[0].current;

        for (int i = 1; i < servers_number; ++i) {
            power = servers[i].power - abs(min_current - servers[i].current);
            if (power < min_power) {
                min_power = (power + min_power)/2;
                if (min_current <= servers[i].current)
                    min_current = -servers[i].power
                                    + min_power + servers[i].current;
                else
                    min_current = servers[i].power
                                    - min_power + servers[i].current;
            }
        }

        return min_power;
    }

    void print_output(double result) {
        ofstream fout("servere.out");
        fout << fixed << setprecision(1) << result << "\n";
        fout.close();
    }
};

int main() {
    TaskServere *task = new (nothrow) TaskServere();
    if (!task) {
        cerr << "Couldn't create a task";
        return -1;
    }
    task->solve();
    delete task;
    return 0;
}
