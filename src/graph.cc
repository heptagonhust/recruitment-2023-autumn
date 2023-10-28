#include "graph.hh"
#include <string.h>
#include <fstream>
#include <cassert>

Graph::Graph(std::string &filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    assert(in);
    in.read(reinterpret_cast<char *>(&vertex_num_), sizeof(int));
    Distance_ = new int[vertex_num_ * vertex_num_];
    for (int i = 0; i < vertex_num_ * vertex_num_; ++i) {
        in.read(reinterpret_cast<char *>(&Distance_[i]), sizeof(int));
    }
    in.close();
}

Graph::Graph(const Graph& g) : vertex_num_(g.vertex_num_), 
                               Distance_(new int[g.vertex_num_ * g.vertex_num_]) {
    memcpy(Distance_, g.Distance_, sizeof(int) * g.vertex_num_ * g.vertex_num_);
}

Graph& Graph::operator=(const Graph& rhs) {
    if (this == &rhs) {
        return *this;
    }
    vertex_num_ = rhs.vertex_num_;
    delete Distance_;
    Distance_ = new int[vertex_num_ * vertex_num_];
    memcpy(Distance_, rhs.Distance_, sizeof(int) * vertex_num_ * vertex_num_);
    return *this;
}

Graph::~Graph() {
    delete Distance_;
}

bool Graph::operator==(const Graph& g) const {
    if (vertex_num_ != g.vertex_num_) {
        return false;
    }
    for (int i = 0; i < vertex_num_ * vertex_num_; ++i) {
        if (Distance_[i] != g.Distance_[i]) {
            return false;
        }
    }
    return true;
}
