#include <bits/stdc++.h>
using namespace std;

struct word {
    int freq[26] = {0};
    int length = 0;
};

class TaskCriptat {
 public:
    void solve() {
        read_input();
        print_output(get_solution());
    }

 private:
    int words_number;
    vector<word> words;
    int letters[26] = {0};
    int idx;

    void create_word(string str) {
        struct word new_word;

        new_word.length = str.size();
        for (long unsigned int i = 0; i < str.size(); ++i) {
            new_word.freq[str[i] - 'a']++;
            letters[str[i] - 'a']++;
        }

        words.push_back(new_word);
    }

    void read_input() {
        ifstream fin("criptat.in");
        fin >> words_number;

        string aux;
        for (int i = 0; i < words_number; ++i) {
            fin >> aux;
            create_word(aux);
        }

        fin.close();
    }

    int get_max_password(int character) {
        sort(words.begin(), words.end(), [character](word &w1, word &w2) {
            return w1.freq[character] * w2.length
                    > w2.freq[character] * w1.length;});

        int total_len = 0, total_char = 0;
        int new_len, new_char;

        for (long unsigned int i = 0; i < words.size(); ++i) {
            new_len = total_len + words[i].length;
            new_char = total_char + words[i].freq[character];

            if (2 * new_char > new_len) {
                total_len = new_len;
                total_char = new_char;
            }
        }

        return total_len;
    }

    int get_solution() {
        int max_app = 0, app;
        for (int i = 0; i < 26; ++i) {
            if (letters[i] != 0) {
                idx = i;
                app = get_max_password(i);
                if (app > max_app) {
                    max_app = app;
                }
            }
        }
        return max_app;
    }

    void print_output(int result) {
        ofstream fout("criptat.out");
        fout << result;
        fout.close();
    }
};

int main() {
    TaskCriptat *task = new (nothrow) TaskCriptat();
    if (!task) {
        cerr << "Couldn't create a task";
        return -1;
    }
    task->solve();
    delete task;
    return 0;
}
