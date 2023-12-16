//debug的过程很久但最终还是没有解决问题，计算结果出错不知道算法哪里出现问题
//请uu们指正

#include "graph.hh"
#include <pthread.h>
#include <iostream>

struct ThreadData {
    Graph* graph;
    int start;
    int end;
    pthread_barrier_t* barrier;
};   //定义一个结构体用于线程的数据传输（gpt的鬼点子，从没尝试过

void* APSP(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    Graph* graph = data->graph;

    for (int k = 0; k < graph->vertex_num(); ++k) {
        pthread_barrier_wait(data->barrier);        //一个临近尾声才理解的bug，对barrier的理解还不够
        for (int i = data->start; i < data->end; ++i) {
            int ik = (*graph)(i, k);               //这里希望减少j循环中对 (*graph)(i, k) 的重复运算
            for (int j = 0; j < graph->vertex_num(); ++j) {
                (*graph)(i, j) = std::min((*graph)(i, j), ik + (*graph)(k, j));
            }
        }
        pthread_barrier_wait(data->barrier); 
    }
    pthread_exit(NULL);
}

Graph Graph::apsp() {
    Graph result(*this);

    int num_threads = 64;

    pthread_t* threads = static_cast<pthread_t*>(malloc(num_threads * sizeof(pthread_t)));
    ThreadData* thread_data = static_cast<ThreadData*>(malloc(num_threads * sizeof(ThreadData)));    
                            //为了编译成功查资料，居然用到了动态内存分配？课上知识复现了属于是
    pthread_barrier_t barrier;

    pthread_barrier_init(&barrier, NULL, num_threads);

    int length = vertex_num_ / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].graph = this;
        thread_data[i].start = i * length;
        thread_data[i].end = (i == num_threads - 1) ? vertex_num_ : (i + 1) * length;
        thread_data[i].barrier = &barrier;

        pthread_create(&threads[i], NULL, APSP, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    free(threads);
    free(thread_data);

    return result;
}
