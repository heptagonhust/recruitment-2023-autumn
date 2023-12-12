#include "graph.hh"
#include<immintrin.h>
#include<pthread.h>
#define min(a,b)    ((a)<(b))?(a):(b)

const int thread_num=110;
struct ARG{
        int *Distance;
        int vertex_num_;
        int rank;};

pthread_barrier_t barrier;              // 循环路障 保证一次k迭代同时更新
void* shorten_path(void *argv);
// ARG argvp[thread_num]=(ARG*)malloc(sizeof(ARG)*thread_num);

Graph Graph::apsp() {
    Graph result(*this);

    int *distance = result.get_raw_ptr();   
    int num=result.vertex_num_;             //循环赋值 作为本地变量保存
    pthread_t threads[thread_num];          // 创建线程对象
    pthread_barrier_init(&barrier,NULL,thread_num); // 路障初始化

    ARG* argvp=(ARG*)malloc(sizeof(ARG)*thread_num);      // 提前分配所有线程所需参数的空间
    for(int i=0;i!=thread_num;++i)
    {
        argvp[i].rank=i;
        argvp[i].Distance=distance;
        argvp[i].vertex_num_=num;
   
        pthread_create(&threads[i],NULL,shorten_path,(void*)&argvp[i]);
    }

    for(int i=0;i<thread_num;++i)
    pthread_join(threads[i],NULL);

    free(argvp);        // 回收 动态分配空间 防止内存泄漏
    argvp=nullptr;

    return result;
}

void* shorten_path(void *argv)
{
    // 保存局部本地量 减少访存
    ARG* argvp=reinterpret_cast<ARG*>(argv);
    int id=argvp->rank;
    int num=argvp->vertex_num_;
    int *dis_mat=argvp->Distance;
    int start=id*num/thread_num;
    int end=(id+1)*num/thread_num;

    for(int k=0;k!=num;++k)
        {
            pthread_barrier_wait(&barrier);
                int ij=start*num;
                int ik=start*num+k;
                int kj=k*num;

            for(int i=start;i!=end;++i){
                __m512i dik=_mm512_set1_epi32(*(dis_mat+ik));       // 广播一下 作为比较
                    //int disik=*(dis_mat+ik);
                for(int j=0;j!=num;j+=64){
                    /**
                     * @brief 进行 4*1 loop unrollings 使用 AVX512指令集
                     */
                    __m512i dij= _mm512_load_epi32(dis_mat+ij);
                    __m512i dkj=_mm512_load_epi32(dis_mat+kj);
                    __m512i dikj= _mm512_add_epi32(dik,dkj);
                    __mmask16 mask=_mm512_cmp_epi32_mask(dikj,dij,_MM_CMPINT_LT);
                    _mm512_mask_store_epi32(dis_mat+ij,mask,dikj);
                    //                    *(dis_mat+ij)=min(*(dis_mat+ij),disik+*(dis_mat+kj));
                      dij= _mm512_load_epi32(dis_mat+ij+16);
                      dkj=_mm512_load_epi32(dis_mat+kj+16);
                     dikj= _mm512_add_epi32(dik,dkj);
                     mask=_mm512_cmp_epi32_mask(dikj,dij,_MM_CMPINT_LT);
                    _mm512_mask_store_epi32(dis_mat+ij+16,mask,dikj);
                    //
                     dij= _mm512_load_epi32(dis_mat+ij+32);
                      dkj=_mm512_load_epi32(dis_mat+kj+32);
                     dikj= _mm512_add_epi32(dik,dkj);
                     mask=_mm512_cmp_epi32_mask(dikj,dij,_MM_CMPINT_LT);
                    _mm512_mask_store_epi32(dis_mat+ij+32,mask,dikj);
                    //
                     dij= _mm512_load_epi32(dis_mat+ij+48);
                      dkj=_mm512_load_epi32(dis_mat+kj+48);
                     dikj= _mm512_add_epi32(dik,dkj);
                     mask=_mm512_cmp_epi32_mask(dikj,dij,_MM_CMPINT_LT);
                    _mm512_mask_store_epi32(dis_mat+ij+48,mask,dikj);
                ij+=64;kj+=64;
        }
            ik+=num;
            kj=k*num;
        }
}

    //argv=nullptr;
    return NULL;
}

