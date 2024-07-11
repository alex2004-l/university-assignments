#include <bits/stdc++.h>
using namespace std;

#define MIN(a, b) (a < b)? a : b
#define MIN3(a, b, c) (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c)

class TaskOferta {
 public:
    void solve() {
        read_input();
        print_output(get_solution());
    }

 private:
    int n;
    int k;
    double sum;

    vector<double> prices;

    void read_input() {
        ifstream fin("oferta.in");

        fin >> n >> k;

        for (int i = 0, aux; i < n; ++i) {
            fin >> aux;
            prices.push_back(aux);
        }

        fin.close();
    }

    void oferta(vector<double> &two_products_sale,
        vector<double> &three_products_sale, vector<double> &dp) {
        dp[0] = sum;
        dp[1] = dp[0];
        dp[2] = MIN(dp[1], dp[0] - two_products_sale[1]);

        double no_sale, two_sale, three_sale;
        for (int i = 3; i <= n; ++i) {
            no_sale = dp[i-1];
            two_sale = dp[i-2] - two_products_sale[i-1];
            three_sale = dp[i-3] - three_products_sale[i-1];
            dp[i] = MIN3(no_sale, two_sale, three_sale);
        }
    }

    double get_solution() {
        vector<double> two_products_sale(n, 0.0);
        vector<double> three_products_sale(n, 0.0);
        vector<double> dp(n+1, 0.0);

        for (int i = 1; i < n; ++i) {
            two_products_sale[i] = (MIN(prices[i-1], prices[i])) / 2.0;
        }

        for (int i = 2; i < n; ++i) {
            three_products_sale[i] = MIN3(prices[i-2], prices[i-1], prices[i]);
        }

        sum = 0;
        for (long unsigned int i = 0; i < prices.size(); ++i) {
            sum += prices[i];
        }

        oferta(two_products_sale, three_products_sale, dp);

        return dp[n];
    }

    void print_output(double result) {
        ofstream fout("oferta.out");
        fout << fixed << setprecision(1) << result << "\n";
        fout.close();
    }
};

int main() {
    TaskOferta *task = new (nothrow) TaskOferta();
    if (!task) {
        cerr << "Couldn't create a task";
        return -1;
    }
    task->solve();
    delete task;
    return 0;
}
