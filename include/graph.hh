#pragma once
#include <string>
#include <memory>
#include <cstdlib>

class Graph
{
    static constexpr int alignment_ = 64;

    int vertex_num_;
    int mem_size_;
    // int *Distance_;
    std::unique_ptr<int[]> Distance_ptr_;

public:
    Graph() = delete;
    Graph(int num) : vertex_num_(num),
                     mem_size_((num * num * sizeof(int) + alignment_ - 1) / alignment_ * alignment_),
                     Distance_ptr_(static_cast<int *>(aligned_alloc(alignment_, mem_size_))) {}
    Graph(const std::string &filename);
    Graph(const Graph &g);
    Graph &operator=(const Graph &rhs);

    Graph(Graph &&g) noexcept;
    Graph &operator=(Graph &&rhs) noexcept;

    ~Graph() = default;

    inline int vertex_num() const { return vertex_num_; }
    inline int &operator()(int i, int j) const { return Distance_ptr_[i * vertex_num_ + j]; }
    inline int *get_raw_ptr() const { return Distance_ptr_.get(); }
    bool operator==(const Graph &g) const;
    bool operator!=(const Graph &g) const { return !(*this == g); }

    Graph apsp();
};