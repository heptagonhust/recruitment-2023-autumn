#include "graph.hh"
#include <string.h>
#include <fstream>
#include <cassert>
#include <iostream>

Graph::Graph(const std::string& filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        exit(-1);
    }
    in.read(reinterpret_cast<char *>(&vertex_num_), sizeof(int));
    mem_size_ = (vertex_num_ * vertex_num_ * sizeof(int) + alignment_ - 1) / alignment_ * alignment_;
    Distance_ptr_.reset(static_cast<int *>(aligned_alloc(alignment_, mem_size_)));
    for (int i = 0; i < vertex_num_ * vertex_num_; ++i) {
        in.read(reinterpret_cast<char *>(&Distance_ptr_[i]), sizeof(int));
    }
    in.close();
}

Graph::Graph(const Graph& g) : vertex_num_(g.vertex_num_), 
                               mem_size_(g.mem_size_),
                               Distance_ptr_(static_cast<int *>(aligned_alloc(alignment_, g.mem_size_))) {
    memcpy(Distance_ptr_.get(), g.Distance_ptr_.get(), sizeof(int) * vertex_num_ * vertex_num_);
}


Graph& Graph::operator=(const Graph& rhs) {
    if (this != &rhs) {
        vertex_num_ = rhs.vertex_num_;
        mem_size_ = rhs.mem_size_;
        Distance_ptr_.reset(static_cast<int *>(aligned_alloc(alignment_, mem_size_)));
        memcpy(Distance_ptr_.get(), rhs.Distance_ptr_.get(), sizeof(int) * vertex_num_ * vertex_num_);
    }
    return *this;
}

Graph::Graph(Graph &&g) noexcept : vertex_num_(g.vertex_num_), mem_size_(g.mem_size_), Distance_ptr_(std::move(g.Distance_ptr_)) {
    g.vertex_num_ = 0;
    g.mem_size_ = 0;
}

Graph& Graph::operator=(Graph &&rhs) noexcept {
    if (this != &rhs) {
        vertex_num_ = rhs.vertex_num_;
        mem_size_ = rhs.mem_size_;
        Distance_ptr_ = std::move(rhs.Distance_ptr_);
        rhs.vertex_num_ = 0;
        rhs.mem_size_ = 0;
    }
    return *this;
}

bool Graph::operator==(const Graph& g) const {
    if (vertex_num_ != g.vertex_num_) {
        return false;
    }
    for (int i = 0; i < vertex_num_ * vertex_num_; ++i) {
        if (Distance_ptr_[i] != g.Distance_ptr_[i]) {
            return false;
        }
    }
    return true;
}
