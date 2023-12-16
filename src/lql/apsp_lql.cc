#include <iostream>
#include <thread>
#include <vector>
#include "graph.hh"

void apsp_func(Graph& graph, int k, int start, int end) {
    int *Distance=graph.get_raw_ptr();
    int ij = 0, kj = 0;
    for (int i = start; i < end; ++i) {
        ij=i*(graph.vertex_num());
        kj=k*(graph.vertex_num());
        int Distance_ik = Distance[i*graph.vertex_num()+k];
        for (int j = 0; j < graph.vertex_num(); ++j) {
            graph(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
            ++ij;
            ++kj;
        }
    }
}

Graph Graph::apsp() {
    Graph result(*this);
    //线程数量
    const int numThreads = 16; 
    //任务分配
    int rowsPerThread = (vertex_num_ + numThreads - 1) / numThreads; // 每个线程处理的行数
    std::vector<std::thread> threads;
    for (int k = 0; k < vertex_num_; ++k) {
        for (int t = 0; t < numThreads; ++t) {
            int start = t * rowsPerThread;
            int end = std::min((t + 1) * rowsPerThread, vertex_num_);
            threads.emplace_back(apsp_func, std::ref(result), k, start, end);
        }
        for (auto& thread : threads) {
            thread.join();
        }
        threads.clear();
    }
    return result;
}