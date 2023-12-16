#includ "graph.hh"
#include <omp.h>

Graph Graph::apsp() {
   Graph result(*this);
   omp_set_num_threads(64);
   for (int k = 0; k < vertex_num_; ++k) {
        #pragma omp parallel for 
       for (int i = 0; i < vertex_num_; ++i) {
            #pragma omp simd
           for (int j = 0; j < vertex_num_; ++j) {
               result(i, j) = min(result(i, j), result(i, k) + result(k, j));
           }
//啃了两天openMP，后面看要求发现居然不让用。
//用Pthread的改好的代码一直调不动，然后就想着要不要换一条路子，尝试一下openMP，就简单写了一下
//在VScode上没有配好openMP的环境，尝试失败 悲、
