#include "graph.hh"
#include <omp.h>

// opt_example
// Graph Graph::apsp()
// {
//     Graph result(*this);
//     int *Distance = result.get_raw_ptr();
//     int ij = 0, ik = 0, kj = 0;
//     for (int k = 0; k < vertex_num_; ++k)
//     {
//         ij = 0;
//         ik = k;
//         for (int i = 0; i < vertex_num_; ++i)
//         {
//             kj = k * vertex_num_;
//             int Distance_ik = Distance[ik];
//             for (int j = 0; j < vertex_num_; ++j)
//             {
//                 result(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
//                 ++ij;
//                 ++kj;
//             }
//             ik += vertex_num_;
//         }
//     }
//     return result;
// }

// openMP + O2 优化
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

// First
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

Graph Graph::apsp()
{
    Graph result(*this);
    int *Distance = result.get_raw_ptr();
    int ij = 0, ik = 0, kj = 0;
    for (int k = 0; k < vertex_num_; ++k)
    {
        ik = k;
        ij = 0;
        for (int i = 0; i < vertex_num_; ++i)
        {
            // int result_ik = result(i, k);
            kj = k * vertex_num_;
            int Distance_ik = Distance[ik];
            for (int j = 0; j < vertex_num_; ++j)
            {

                // int new_dist = result_ik + result(k, j);
                // if (new_dist < result(i, j))
                // {
                //     result(i, j) = new_dist;
                // }
                int new_dist = Distance_ik + Distance[kj];
                if (new_dist < Distance[ij])
                {
                    result(i, j) = new_dist;
                }
                ij++;
                kj++;
            }
            ik += vertex_num_;
            // ij += vertex_num_;
        }
    }

    return result;
}
