#include "graph.hh"
#include<immintrin.h>
#include<pthread.h>
#define min(a,b)    ((a)<(b))?(a):(b)

const int thread_num=64;
struct ARG{
        int *Distance;
        int vertex_num_;
        int rank;};
      //  Graph *pts;};

pthread_barrier_t barrier;
void* shorten_path(void *argv);

Graph Graph::apsp() {
    Graph result(*this);
    //int *Distance = result.get_raw_ptr();

    pthread_t threads[thread_num];          // 创建线程对象
    pthread_barrier_init(&barrier,NULL,thread_num);
   
    for(int i=0;i!=thread_num;++i)
    {
        auto argvp=(ARG*)malloc(sizeof(ARG));
        argvp->rank=i;
        argvp->Distance=result.get_raw_ptr();
        argvp->vertex_num_=result.vertex_num_;
   //     argvp->pts=&result;
        pthread_create(&threads[i],NULL,shorten_path,(void*)argvp);
    }

    for(int i=0;i<thread_num;++i)
    pthread_join(threads[i],NULL);

    return result;
}

void* shorten_path(void *argv)
{
    auto argvp=reinterpret_cast<ARG*>(argv);
    int id=argvp->rank;
    int num=argvp->vertex_num_;
    int *dis_mat=argvp->Distance;
    int start=id*num/thread_num;
    int end=(id+1)*num/thread_num;

  //  auto result=argvp->pts;
    for(int k=0;k!=num;++k)
        {
            pthread_barrier_wait(&barrier);
            int ij=start;
            int ik=start+k;
            int kj=k*num;

            for(int i=start;i!=end;++i)
            {
                int distance_ik=*(dis_mat+ik);
                for(int j=0;j!=num;++j){
                    *(dis_mat+ij)=min(*(dis_mat+ij),*(dis_mat+ik)+*(dis_mat+kj));
                    ++ij;++kj;
                    //if(dis_mat[i][j]>dis_mat[i][k]+dis_mat[k][j])
                }
                ik+=num;
        }
}
    free(argv);
    argv=nullptr;

    return NULL;
}

//   int ij = 0, ik = 0, kj = 0;
//     for (int k = 0; k < vertex_num_; ++k) {
//         ij = 0;
//         ik = k;
//         kj = k * vertex_num_;

//         for (int i = 0; i < vertex_num_; ++i) {
//             int Distance_ik = Distance[ik];
//             for (int j = 0; j < vertex_num_; ++j) {
//                 result(i, j) = min(Distance[ij], Distance_ik + Distance[kj]);
//                 ++ij;
//                 ++kj;
//             }
//             ik += vertex_num_;
//         }
//     }