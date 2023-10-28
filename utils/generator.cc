#include "graph.hh"
#include <iostream>
#include <fstream>
#include <limits>
#include <random>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: ./generator <vertex_num> <raw_filename> <check_filename>" << endl;
        return 0;
    }
    int vertex_num = atoi(argv[1]);
    string raw_filename = argv[2];
    ofstream out_raw(raw_filename, ios::out | ios::binary);
    out_raw.write(reinterpret_cast<char *>(&vertex_num), sizeof(int));

    static default_random_engine e;
    uniform_int_distribution<int> u(0, numeric_limits<int>::max() / vertex_num);
    Graph g(vertex_num);
    for (int i = 0; i < vertex_num; ++i) {
        for (int j = 0; j < vertex_num; ++j) {
            int tmp = (i == j) ? 0 : u(e);
            g(i, j) = tmp;
            out_raw.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        }
    }
    out_raw.close();

    Graph check_data = g.apsp();
    string check_filename = argv[3];
    ofstream out_check(check_filename, ios::out | ios::binary);
    out_check.write(reinterpret_cast<char *>(&vertex_num), sizeof(int));
    for (int i = 0; i < vertex_num; ++i) {
        for (int j = 0; j < vertex_num; ++j) {
            int tmp = check_data(i, j);
            out_check.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        }
    }
    out_check.close();
    return 0;
}