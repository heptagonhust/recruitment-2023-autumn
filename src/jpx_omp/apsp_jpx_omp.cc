#include "graph.hh"
#include<omp.h>

#define min(a,b)    ((a)<(b))?(a):(b)


Graph Graph::apsp() {
   Graph result(*this);
   omp_set_num_threads(64);
   for (int k = 0; k < vertex_num_; ++k) {
        #pragma omp parallel for schedule(dynamic)
       for (int i = 0; i < vertex_num_; ++i) {
            #pragma unroll
            #pragma omp simd
           for (int j = 0; j < vertex_num_; ++j) {
               result(i, j) = min(result(i, j), result(i, k) + result(k, j));
           }
       }
   }
   return result;
}
