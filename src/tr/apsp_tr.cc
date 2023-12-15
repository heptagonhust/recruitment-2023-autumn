#include "graph.hh"
#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <immintrin.h>
void Floyd(Graph &target, int start, int end, int k)
{
    int *Distance = target.get_raw_ptr();

    int ij = 0, ik = 0, kj = 0;
    int vertex_num_ = target.vertex_num();

    ij = start * vertex_num_;
    ik = start * vertex_num_ + k;
    int new_dist;
    for (int i = start; i < end; ++i)
    {
        kj = k * vertex_num_;
        int Distance_ik = Distance[ik];
        int j = 0;
        for (; j < vertex_num_ - 8; j += 8)
        {
            __m256i Distance_kj = _mm256_load_si256((__m256i *)(Distance + kj));
            __m256i Dis = _mm256_set1_epi32(Distance_ik);
            Dis = _mm256_add_epi32(Dis, Distance_kj);
            __m256i Distance_ij = _mm256_load_si256((__m256i *)(Distance + ij));
            Dis = _mm256_min_epi32(Distance_ij, Dis);
            _mm256_storeu_si256((__m256i *)(Distance + ij), Dis);
            ij += 8;
            kj += 8;
        }
        for (; j < vertex_num_; ++j)
        {

            new_dist = Distance_ik + Distance[kj];
            if (new_dist < Distance[ij])
            {
                target(i, j) = new_dist;
            }
            ij++;
            kj++;
        }
        ik += vertex_num_;
    }
}

Graph Graph::apsp()
{
    Graph result(*this);
    std::vector<std::thread> threads;

    // const int Num_threads = 110;//调整线程数量，自动分配各线程数据量
    // int thread_len = vertex_num_ / Num_threads;
    const int thread_len = 64; // 提前分配数据量，自动分配线程数量
    int Num_threads = vertex_num_ % thread_len == 0 ? vertex_num_ / thread_len : vertex_num_ / thread_len + 1;

    for (int k = 0; k < vertex_num_; k++)
    {
        for (int thread = 0; thread < Num_threads; ++thread)
        {
            int start = thread * thread_len;
            int end = (thread == Num_threads - 1) ? vertex_num_ : (thread + 1) * thread_len;
            threads.emplace_back(Floyd, std::ref(result), start, end, k);
        }
        for (auto &t : threads)
        {
            t.join();
        }

        // 清空线程容器，准备下一轮
        threads.clear();
    }
    return result;
}
