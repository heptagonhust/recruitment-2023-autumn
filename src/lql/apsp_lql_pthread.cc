#include "graph.hh"
#include <pthread.h>
#include <vector>

//传参结构体
struct ThreadData {
    Graph *graph;
    int start;
    int end;
    int k;
};
const int numThreads = 16;           // 设置线程数量

void* apsp_func(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    Graph* graph = data->graph;
    int start = data->start;
    int end = data->end;
    int k=data->k;
    //分块处理，可以不用使用线程锁
    int *Distance=graph->get_raw_ptr();
    int ij = 0, kj = 0;
    for (int i = start; i < end; ++i) {
        kj=k*(graph->vertex_num());
        ij=i*(graph->vertex_num());
        int Distance_ik = Distance[i*(graph->vertex_num())+k];
        for (int j = 0; j < graph->vertex_num(); ++j) {
            (*graph)(i, j) = std::min(Distance[ij], Distance_ik + Distance[kj]);
            ++ij;
            ++kj;
        }
    }
    pthread_exit(0);
}
Graph Graph::apsp() {
    Graph result(*this);
    // 创建线程数据和线程对象
    pthread_t threads[numThreads];
    std::vector<ThreadData> threadData(numThreads);
    // 分配任务给每个线程
    int load_size = vertex_num_ / numThreads;
    int loss_size = vertex_num_ % numThreads;
    for(int k=0;k<vertex_num_;k++)
    {
        int start=0;
        for (int i = 0; i < numThreads; ++i)
        {        
            int end = start + load_size;
            if (loss_size > 0) {
                end++;
                loss_size--;
            }
            threadData[i].graph = &result;
            threadData[i].start = start;
            threadData[i].end = end;
            threadData[i].k=k;
            pthread_create(&threads[i], NULL, apsp_func, (void *)&threadData[i]);
        }
    } 
    // 等待所有线程完成
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    return result;
}