#include "graph.hh"

Graph Graph::apsp() {
    Graph result(*this);
    int *Distance = result.get_raw_ptr();
    int ij = 0, ik = 0, kj = 0;
    for (int k = 0; k < vertex_num_; ++k) {
        ij = 0;
        ik = k;
        for (int i = 0; i < vertex_num_; ++i) {
            kj = k * vertex_num_;
            int Distance_ik = Distance[ik];
            for (int j = 0; j < vertex_num_; ++j) {
                result(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
                ++ij;
                ++kj;
            }
            ik += vertex_num_;
        }
    }
    return result;
}
