#include <bits/stdc++.h>
using namespace std;

class TaskCompresie {
 public:
    void solve() {
        read_input();
        print_output(get_solution());
    }

 private:
    int dimension_A;
    int dimension_B;

    vector<int> array_A;
    vector<int> array_B;

    void read_input() {
        ifstream fin("compresie.in");

        fin >> dimension_A;
        for (int i = 0, aux; i < dimension_A; ++i) {
            fin >> aux;
            array_A.push_back(aux);
        }

        fin >> dimension_B;
        for (int i = 0, aux; i < dimension_B; ++i) {
            fin >> aux;
            array_B.push_back(aux);
        }

        fin.close();
    }

    int get_solution() {
        unsigned long long sumA = 0, sumB = 0;
        int cnt = 0, idxA = 0, idxB = 0;
        while (idxA < dimension_A && idxB < dimension_B) {
            if (sumA == sumB && sumA != 0) {
                cnt ++;
                sumA = sumB = 0;
            }
            if (sumA <= sumB) {
                sumA += array_A[idxA++];
            } else {
                sumB += array_B[idxB++];
            }
        }
        while (idxA < dimension_A) {
            sumA += array_A[idxA++];
        }
        while (idxB < dimension_B) {
            sumB += array_B[idxB++];
        }

        return (sumA == sumB) ? (cnt + 1) : -1;
    }

    void print_output(int result) {
        ofstream fout("compresie.out");
        fout << result;
        fout.close();
    }
};

int main() {
    TaskCompresie *task = new (nothrow) TaskCompresie();
    if (!task) {
        cerr << "Couldn't create a task";
        return -1;
    }
    task->solve();
    delete task;
    return 0;
}
