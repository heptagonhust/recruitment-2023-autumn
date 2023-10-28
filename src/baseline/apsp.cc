#include "graph.hh"

Graph Graph::apsp() {
   Graph result(*this);
   for (int k = 0; k < vertex_num_; ++k) {
       for (int i = 0; i < vertex_num_; ++i) {
           for (int j = 0; j < vertex_num_; ++j) {
               result(i, j) = std::min(result(i, j), result(i, k) + result(k, j));
           }
       }
   }
   return result;
}
