#include "graph.hh"

// opt_example
/*
Graph Graph::apsp()
{
    Graph result(*this);
    int *Distance = result.get_raw_ptr();
    int ij = 0, ik = 0, kj = 0;
    for (int k = 0; k < vertex_num_; ++k)
    {
        ij = 0;
        ik = k;
        for (int i = 0; i < vertex_num_; ++i)
        {
            kj = k * vertex_num_;
            int Distance_ik = Distance[ik];
            for (int j = 0; j < vertex_num_; ++j)
            {
                result(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
                ++ij;
                ++kj;
            }
            ik += vertex_num_;
        }
    }
    return result;
}
*/

// openMP + O2 优化
/*
//  Graph Graph::apsp()
//  {
//      Graph result(*this);

// #pragma omp parallel for
//     for (int k = 0; k < vertex_num_; ++k)
//     {
// #pragma omp parallel for
//         for (int i = 0; i < vertex_num_; ++i)
//         {
//             for (int j = 0; j < vertex_num_; ++j)
//             {
//                 result(i, j) = std::min(result(i, j), result(i, k) + result(k, j));
//             }
//         }
//     }

//     return result;
// }

*/

// First
/*
// Graph Graph::apsp()
// {
//     Graph result(*this);
//     for (int k = 0; k < vertex_num_; ++k)
//     {
//         for (int i = 0; i < vertex_num_; ++i)
//         {
//             int result_ik = result(i, k);

//             for (int j = 0; j < vertex_num_; ++j)
//             {
//                 // Minimize the number of function calls
//                 int new_dist = result_ik + result(k, j);
//                 // int &target = result(i, j);
//                 // if (new_dist < target)
//                 //     target = new_dist;
//                 if (new_dist < result(i, j))
//                 {
//                     result(i, j) = new_dist;
//                 }
//                 // result(i, j) = std::min(result(i, j), result_ik + result(k, j));
//             }
//         }
//     }

//     return result;
// }

*/

// 消除重复运算和不必要的函数调用（最终版）
/*

// Graph Graph::apsp()
// {
//     Graph result(*this);
//     int *Distance = result.get_raw_ptr();
//     int ij = 0, ik = 0, kj = 0;
//     for (int k = 0; k < vertex_num_; ++k)
//     {
//         ik = k;
//         ij = 0;
//         for (int i = 0; i < vertex_num_; ++i)
//         {
//             kj = k * vertex_num_;
//             int Distance_ik = Distance[ik];
//             for (int j = 0; j < vertex_num_; ++j)
//             {
//                 int new_dist = Distance_ik + Distance[kj];
//                 if (new_dist < Distance[ij])
//                 {
//                     result(i, j) = new_dist;
//                 }
//                 ij++;
//                 kj++;
//             }
//             ik += vertex_num_;
//         }
//     }

//     return result;
// }
*/

// struct node
// {
//     Graph &result;
//     int start;
//     int end;
//     node(Graph &temp, int x, int y) : result(temp), start(x), end(y) {}
// };
// pthread_mutex_t mutex;
// void *Floyd(void *rank)
// {
//     node *temp = (node *)rank;
//     Graph &target(temp->result);
//     int start = temp->start;
//     int end = temp->end;
//     int *Distance = target.get_raw_ptr();
//     int ij = 0, ik = 0, kj = 0;
//     int vertex_num_ = target.vertex_num();
//     for (int k = 0; k < vertex_num_; ++k)
//     {
//         ik = k;
//         ij = 0;
//         for (int i = start; i < end; ++i)
//         {
//             kj = k * vertex_num_;
//             int Distance_ik = Distance[ik];
//             for (int j = 0; j < vertex_num_; ++j)
//             {
//                 int new_dist = Distance_ik + Distance[kj];
//                 if (new_dist < Distance[ij])
//                 {
//                     pthread_mutex_lock(&mutex);
//                     target(i, j) = new_dist;
//                     pthread_mutex_unlock(&mutex);
//                 }
//                 ij++;
//                 kj++;
//             }
//             ik += vertex_num_;
//         }
//     }
// }
// Graph Graph::apsp()
// {
//     Graph result(*this);
//     int *Distance = result.get_raw_ptr();
//     const int Num_threads = 2;

//     long thread;
//     pthread_t *thread_handles;
//     thread_handles = (pthread_t *)malloc(Num_threads * sizeof(pthread_t));
//     pthread_mutex_t mutex;
//     for (thread = 0; thread < Num_threads; thread++)
//     {
//         int start = thread * vertex_num_ / Num_threads;
//         int end = (thread + 1) * vertex_num_ / Num_threads;
//         node temp(result, start, end);
//         pthread_create(&thread_handles[thread], NULL, Floyd, (void *)&temp);
//     }
//     for (thread = 0; thread < Num_threads; thread++)
//     {
//         pthread_join(thread_handles[thread], NULL);
//     }
//     free(thread_handles);
//     free(&mutex);
//     return result;
// }

#include <thread>
#include <vector>
#include <mutex>

struct node
{
    Graph &result;
    int start;
    int end;
    node(Graph &temp, int x, int y) : result(temp), start(x), end(y) {}
};

std::mutex mtx;

void Floyd(node *temp)
{
    Graph &target = temp->result;
    int start = temp->start;
    int end = temp->end;
    int *Distance = target.get_raw_ptr();
    int ij = 0, ik = 0, kj = 0;
    int vertex_num_ = target.vertex_num();
    for (int k = 0; k < vertex_num_; ++k)
    {
        ik = k;
        ij = 0;
        for (int i = start; i < end; ++i)
        {
            kj = k * vertex_num_;
            int Distance_ik = Distance[ik];
            for (int j = 0; j < vertex_num_; ++j)
            {
                int new_dist = Distance_ik + Distance[kj];
                // Ensure that target(i, j) is thread-safe before removing the lock
                std::lock_guard<std::mutex> lock(mtx);
                target(i, j) = new_dist;
                ij++;
                kj++;
            }
            ik += vertex_num_;
        }
    }
}

Graph Graph::apsp()
{
    Graph result(*this);
    int *Distance = result.get_raw_ptr();
    const int Num_threads = 2;

    std::vector<std::thread> threads;
    for (int thread = 0; thread < Num_threads; ++thread)
    {
        int start = thread * vertex_num_ / Num_threads;
        int end = (thread + 1) * vertex_num_ / Num_threads;
        node temp(result, start, end);
        threads.emplace_back(Floyd, &temp);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return result;
}
