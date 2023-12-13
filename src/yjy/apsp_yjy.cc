#include "graph.hh"
#include <thread>
#include <immintrin.h>    // AVX
#define MAXTHREADS 512 //这里设置最大线程数，代码会动态设置调用多少线程，所以这里直接设置一个资源最大值——yjy

void funcyjy(int k, int *Distance, int vertex_num_, int n, int total)
{
    int ij = 0, ik = k + n * vertex_num_, kj = 0, DistanceNew = 0;
    for (int i = n; i < vertex_num_; i += total) {
            kj = k * vertex_num_;
            ij = i * vertex_num_;
            int Distance_ik = Distance[ik],j=0;
            for (; j+4 < vertex_num_; j+=4) {
                __m128i Disk =  _mm_load_si128((__m128i*)(Distance + kj));
                __m128i one = _mm_set1_epi32(Distance_ik);
                Disk=_mm_add_epi32(Disk, one);
                __m128i Dis =  _mm_load_si128((__m128i*)(Distance + ij));
                Dis=_mm_min_epi32(Dis,Disk);
                _mm_storeu_si128((__m128i*)(Distance + ij), Dis);
                //DistanceNew = Distance_ik + Distance[kj];
                //Distance[ij] = Distance[ij] < DistanceNew ? Distance[ij] : DistanceNew;
                //result(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
                ij+=4;
                kj+=4;   
            }
            for(; j < vertex_num_ ;++j)
            {
                DistanceNew = Distance_ik + Distance[kj];
                Distance[ij] = Distance[ij] < DistanceNew ? Distance[ij] : DistanceNew;
                //result(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
                ++ij;
                ++kj;   
            }
            ik += vertex_num_ * total;
        }
}


Graph Graph::apsp() {
    Graph result(*this);
    int *Distance = result.get_raw_ptr();
    int Numofthread = vertex_num_/64 < MAXTHREADS? vertex_num_/64:MAXTHREADS; //每线程处理64行
    std::thread n[Numofthread]; //这样用变量定义数组大小居然是可以的
    for (int k = 0; k < vertex_num_; ++k) {
        for(int i = 0; i < Numofthread; i++)
        {
            n[i]=std::thread(funcyjy,k,Distance,vertex_num_,i,Numofthread);
        }
        for(int i = 0; i < Numofthread; i++)
        {
            n[i].join();
        }
        
        
    }
    return result;
}
