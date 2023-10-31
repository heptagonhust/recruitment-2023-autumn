#include "graph.hh"
#include "timer.hh"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./apsp_test <in_file> <check_file>" << endl;
        return -1;
    }
    string in_file(argv[1]);
    string check_file(argv[2]);
    Graph g(in_file);
    Graph check_data(check_file);
    Graph res(g.vertex_num());
    {
        string test_name = string("TestCase ") + to_string(g.vertex_num());
        ScopeTimer timer(test_name);
        res = g.apsp();
    }

    if (res == check_data) return 0;
    else {
        cout << "Results not match!" << endl;
        return -1;
    }
}