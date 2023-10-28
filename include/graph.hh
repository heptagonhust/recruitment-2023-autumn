#pragma once
#include <string>

class Graph {
    int vertex_num_;
    int *Distance_;

public:
    Graph() = delete;
    Graph(int num) : vertex_num_(num),
                        Distance_(new int[num * num]) {}
    Graph(std::string &filename);
    Graph(const Graph& g);
    Graph& operator=(const Graph& rhs);
    ~Graph();

    inline int vertex_num() const { return vertex_num_; }
    inline int& operator()(int i, int j) const { return Distance_[i * vertex_num_ + j]; }
    bool operator==(const Graph& g) const;

    Graph apsp();
};