#include <bits/stdc++.h>
using namespace std;

#define MOD 1000000007

class TaskColorare {
 public:
    void solve() {
        read_input();
        print_output(get_solution());
    }

 private:
    struct Block {
        int number;
        char orientation;
    };

    int blocks_number;
    vector<Block> blocks;

    void read_input() {
        ifstream fin("colorare.in");
        fin >> blocks_number;

        Block aux;
        for (int i = 0; i < blocks_number; ++i) {
            fin >> aux.number >> aux.orientation;
            blocks.push_back(aux);
        }
        fin.close();
    }

    unsigned long long pow_MOD(unsigned long long base, int power) {
        unsigned long long result = 1;
        while (power > 0) {
            if (power % 2 != 0)
                result = (result * base) % MOD;
            power = power/2;
            base = (base * base) % MOD;
        }
        return result;
    }

    unsigned long long get_solution() {
        unsigned long long result;
        if (blocks[0].orientation == 'H') {
            result = 6 * pow_MOD(3, blocks[0].number - 1) % MOD;
        } else {
            result = 3 * pow_MOD(2, blocks[0].number - 1) % MOD;
        }

        for (int i = 1; i < blocks_number; ++i) {
            if (blocks[i].orientation == 'H') {
                if (blocks[i-1].orientation == 'H')
                    result = result * pow_MOD(3, blocks[i].number) % MOD;
                else
                    result = result * 2
                                * pow_MOD(3, blocks[i].number - 1) % MOD;
            } else {
                if (blocks[i-1].orientation == 'H')
                    result = result * pow_MOD(2, blocks[i].number - 1) % MOD;
                else
                    result = result * pow_MOD(2, blocks[i].number) % MOD;
            }
        }
        return result;
    }

    void print_output(unsigned long long result) {
        ofstream fout("colorare.out");
        fout << result;
        fout.close();
    }
};

int main() {
    TaskColorare *task = new (nothrow) TaskColorare();
    if (!task) {
        cerr << "Couldn't create a task";
        return -1;
    }
    task->solve();
    delete task;
    return 0;
}
