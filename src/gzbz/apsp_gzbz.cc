#include "graph.hh"
#include<iostream>
#include <thread>
#include <vector>
#include <immintrin.h>
///////////
#include <future>
#include <functional>
#include <queue>


void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
   for (int i = start_idx; i < end_idx; ++i) {
        int result_ik = result(i, k);
        __m256i result_ik_vec = _mm256_set1_epi32(result_ik);
        for (int j = 0; j < vertex_num; j += 8) {
            __m256i result_ij_vec = _mm256_loadu_si256((__m256i*)&result(i, j));
            __m256i result_kj_vec = _mm256_loadu_si256((__m256i*)&result(k, j));  
            __m256i sum_vec = _mm256_add_epi32(result_ik_vec, result_kj_vec);
            __m256i min_vec = _mm256_min_epi32(result_ij_vec, sum_vec);
            _mm256_storeu_si256((__m256i*)&result(i, j), min_vec);
        }
    }
}     
Graph Graph::apsp() {
   Graph result(*this);
   const int num_threads = 8 ; 
   std::vector<std::thread> threads(num_threads);
   const int chunk_size = vertex_num_ / num_threads;
   for (int k = 0; k < vertex_num_; ++k) {
       for (int t = 0; t < num_threads; ++t) {
            int start_idx = t * chunk_size;
            int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
            threads[t] = std::thread(apsp_t, std::ref(result), start_idx, end_idx, k, vertex_num_);
       }
       for (auto& thread : threads) {
           thread.join();
       }
   }
   return result;
}
// //0.提前计算一步操作
// Graph Graph::apsp() {
//    Graph result(*this);
//    for (int k = 0; k < vertex_num_; ++k) {
//        for (int i = 0; i < vertex_num_; ++i) {
//            int result_ik = result(i, k);  // 提前获取 result(i, k) 的值，避免重复计算
//            for (int j = 0; j < vertex_num_; ++j) {
//                result(i, j) = std::min(result(i, j), result_ik + result(k, j));
//            }
//        }
//    }
//    return result;
// }
/***************************************************
Baseline on 128: 约 0.2 s
Baseline on 512: 约 12.5 s
Baseline on 1024: 约 99.7 s
Baseline on 4096: 约 6640 s
0.提前计算一步操作                                      128[191ms]      512[11.3s]      1024[]          4096[]
1.仅用cpu多线程操作优化              num_threads = 16:  128[105ms]      512[1.7s]       1024[11.3s]     4096[]
2.仅用SIMD128向量操作优化                               128[52ms]       512[2.4s]       1024[]          4096[]
3.用SIMD向量+多线程操作优化          num_threads = 16:  128[90ms]       512[925ms]      1024[4.4s]      4096[]
4.使用AVX指令集进行优化                                 128[34ms]       512[1.2s]       1024[9.5s]      4096[]
5.用AVX向量+多线程操作优化           num_threads = 4:   128[46ms]       512[857ms]      1024[5.6s]      4096[197.2s]
                                    num_threads = 8:   128[51ms]       512[605ms]      1024[3.1s]      4096[121.5s]
                                    num_threads = 16:  128[55ms]       512[526ms]      1024[2.2s]      4096[81.9s]
                                    num_threads = 32:  128[124ms]      512[579ms]      1024[1.9s]      4096[67.5s]  
                                    num_threads = 64:  128[236ms]       512[1.1ms]      1024[2.5s]     4096[58.3s]
6.内存对齐（舍弃）   加锁也时间加长了
7.用线程池进行优化                   num_threads = 4:   128[25ms]       512[405ms]      1024[2.8s]      4096[159.3s]
                                    num_threads = 8:   128[24ms]       512[288ms]      1024[1.8s]      4096[82.8s]
                                    num_threads = 64:  128[59ms]       512[254ms]      1024[830ms]     4096[20.6s]
8.gpu。。。
****************************************************/





//1.仅用cpu多线程操作优化       num_threads = 16:   128[105ms]  512[1.7s]  1024[11.3s]    4096[]
// void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
//     for (int i = start_idx; i < end_idx; ++i) {
//         int result_ik = result(i, k);
//         for (int j = 0; j < vertex_num; ++j) {
//             result(i, j) = std::min(result(i, j), result_ik + result(k, j));
//         }
//     }
// }
// Graph Graph::apsp() {
//    Graph result(*this);
//    const int num_threads = 16;
//    std::vector<std::thread> threads(num_threads);
//    const int chunk_size = vertex_num_ / num_threads;
//    for (int k = 0; k < vertex_num_; ++k) {
//        for (int t = 0; t < num_threads; ++t) {
//             int start_idx = t * chunk_size;
//             int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
//             threads[t] = std::thread(apsp_t, std::ref(result), start_idx, end_idx, k, vertex_num_);
//        }
//        for (auto& thread : threads) {
//            thread.join();
//        }
//    }
//    return result;
// }



// //2.仅用SIMD向量操作优化            128[52ms]  512[2.4s]  1024[]    4096[]
// Graph Graph::apsp() {
//     Graph result(*this); 
//     for (int k = 0; k < vertex_num_; ++k) {
//         for (int i = 0; i < vertex_num_; ++i) {
//             __m128i val_ik = _mm_set1_epi32(result(i, k)); // 将result(i, k)复制为4个整数
//             for (int j = 0; j < vertex_num_; j += 4) {
//                 __m128i val_kj = _mm_loadu_si128((__m128i*)&result(k, j)); 
//                 __m128i val_ij = _mm_loadu_si128((__m128i*)&result(i, j)); 
//                 __m128i sum = _mm_add_epi32(val_ik, val_kj);
//                 __m128i min_val = _mm_min_epi32(val_ij, sum); 
//                 _mm_storeu_si128((__m128i*)&result(i, j), min_val); 
//             }
//             for (int j = vertex_num_ & ~3; j < vertex_num_; ++j) {// 处理剩余的不足4个元素的情况
//                 result(i, j) = std::min(result(i, j), result(i, k) + result(k, j));
//             }
//         }
//     }
//     return result;
// }


// //3.用SIMD向量+多线程操作优化        num_threads = 16:   128[90ms]  512[925ms]  1024[4.4s]    4096[]
// void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
//    for (int i = start_idx; i < end_idx; ++i) {
//         __m128i val_ik = _mm_set1_epi32(result(i, k)); 
//         for (int j = 0; j < vertex_num; j += 4) {
//             __m128i val_ik = _mm_set1_epi32(result(i, k));
//             __m128i val_kj = _mm_loadu_si128((__m128i*)&result(k, j));
//             __m128i val_ij = _mm_loadu_si128((__m128i*)&result(i, j)); 
//             __m128i sum = _mm_add_epi32(val_ik, val_kj);
//             __m128i min_val = _mm_min_epi32(val_ij, sum);
//             _mm_storeu_si128((__m128i*)&result(i, j), min_val); 
//         }
//         for (int j = vertex_num & ~3; j < vertex_num; ++j) {// 处理剩余的不足4个元素的情况
//             result(i, j) = std::min(result(i, j), result(i, k) + result(k, j));
//         }
//     }
// }
// Graph Graph::apsp() {
//     Graph result(*this);
//     const int num_threads = 16; 
//     std::vector<std::thread> threads(num_threads);
//     const int chunk_size = vertex_num_ / num_threads; 
//     for (int k = 0; k < vertex_num_; ++k) {
//         for (int t = 0; t < num_threads; ++t) {
//                 int start_idx = t * chunk_size;
//                 int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
//                 threads[t] = std::thread(apsp_t,std::ref(result), start_idx, end_idx, k, vertex_num_);
//         }
//         for (auto& thread : threads) {
//             thread.join();
//         }
//     }
//     return result;
// }



//4.使用AVX指令集进行优化           128[34ms]  512[1.2s]  1024[9.5s]    4096[]
// Graph Graph::apsp() {
//     Graph result(*this);
//     int vertex_num = vertex_num_;
//     for (int k = 0; k < vertex_num; ++k) {
//         for (int i = 0; i < vertex_num; ++i) {
//             int result_ik = result(i, k);
//             __m256i result_ik_vec = _mm256_set1_epi32(result_ik);
//             for (int j = 0; j < vertex_num; j += 8) {
//                 __m256i result_ij_vec = _mm256_loadu_si256((__m256i*)&result(i, j));
//                 __m256i result_kj_vec = _mm256_loadu_si256((__m256i*)&result(k, j));
//                 __m256i sum_vec = _mm256_add_epi32(result_ik_vec, result_kj_vec);
//                 __m256i min_vec = _mm256_min_epi32(result_ij_vec, sum_vec);
//                 _mm256_storeu_si256((__m256i*)&result(i, j), min_vec);
//             }
//         }
//     }
//     return result;
// }


// 5.用AVX向量+多线程操作优化        num_threads = 4:   128[46ms]   512[857ms]  1024[5.6s]     4096[197.2s]
//                                 num_threads = 8:   128[51ms]   512[605ms]  1024[3.1s]     4096[121.5s]
//                                 num_threads = 16:   128[55ms]   512[526ms]  1024[2.2s]    4096[81.9s]
//                                 num_threads = 32:   128[124ms]  512[579ms]  1024[1.9s]    4096[67.5s]
//                                 num_threads = 64:   128[523ms]  512[1.9ms]  1024[4.3s]    4096[52.8s]        
// void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
//    for (int i = start_idx; i < end_idx; ++i) {
//         int result_ik = result(i, k);
//         __m256i result_ik_vec = _mm256_set1_epi32(result_ik);
//         for (int j = 0; j < vertex_num; j += 8) {
//             __m256i result_ij_vec = _mm256_loadu_si256((__m256i*)&result(i, j));
//             __m256i result_kj_vec = _mm256_loadu_si256((__m256i*)&result(k, j));  
//             __m256i sum_vec = _mm256_add_epi32(result_ik_vec, result_kj_vec);
//             __m256i min_vec = _mm256_min_epi32(result_ij_vec, sum_vec);
//             _mm256_storeu_si256((__m256i*)&result(i, j), min_vec);
//         }
//     }
// }     
// Graph Graph::apsp() {
//    Graph result(*this);
//    const int num_threads = 8 ; 
//    std::vector<std::thread> threads(num_threads);
//    const int chunk_size = vertex_num_ / num_threads;
//    for (int k = 0; k < vertex_num_; ++k) {
//        for (int t = 0; t < num_threads; ++t) {
//             int start_idx = t * chunk_size;
//             int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
//             threads[t] = std::thread(apsp_t, std::ref(result), start_idx, end_idx, k, vertex_num_);
//        }
//        for (auto& thread : threads) {
//            thread.join();
//        }
//    }
//    return result;
// }
    

//6.用内存对齐(舍弃)


/*7.用线程池进行优化              
num_threads = 4:   128[25ms]      512[405ms]      1024[2.8s]      4096[159.3s]
num_threads = 8:   128[24ms]      512[288ms]      1024[1.8s]      4096[82.8s]
num_threads = 64:   128[59ms]      512[254ms]      1024[830ms]      4096[20.6s]
*/
// class ThreadPool {
// public:
//     ThreadPool(size_t threads) : stop(false) {
//         for (size_t i = 0; i < threads; ++i) {
//             workers.emplace_back([this] {
//                 while (true) {
//                     std::function<void()> task;
//                     {
//                         std::unique_lock<std::mutex> lock(queue_mutex);
//                         condition.wait(lock, [this] { return stop || !tasks.empty(); });
//                         if (stop && tasks.empty()) {
//                             return;
//                         }
//                         task = std::move(tasks.front());
//                         tasks.pop();
//                     }
//                     task();
//                 }
//             });
//         }
//     }
//     template<class F, class... Args>
//     auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
//         using return_type = typename std::result_of<F(Args...)>::type;
//         auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
//         std::future<return_type> res = task->get_future();
//         {
//             std::unique_lock<std::mutex> lock(queue_mutex);
//             if (stop) {
//                 throw std::runtime_error("enqueue on stopped ThreadPool");
//             }
//             tasks.emplace([task]() { (*task)(); });
//         }
//         condition.notify_one();
//         return res;
//     }
//     ~ThreadPool() {
//         {
//             std::unique_lock<std::mutex> lock(queue_mutex);
//             stop = true;
//         }
//         condition.notify_all();
//         for (std::thread &worker : workers) {
//             worker.join();
//         }
//     }
// private:
//     std::vector<std::thread> workers;
//     std::queue<std::function<void()>> tasks;
//     std::mutex queue_mutex;
//     std::condition_variable condition;
//     bool stop;
// };
// void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
//    for (int i = start_idx; i < end_idx; ++i) {
//         int result_ik = result(i, k);
//         __m256i result_ik_vec = _mm256_set1_epi32(result_ik);
//         for (int j = 0; j < vertex_num; j += 8) {
//             __m256i result_ij_vec = _mm256_loadu_si256((__m256i*)&result(i, j));// 加载数据到AVX向量
//             __m256i result_kj_vec = _mm256_loadu_si256((__m256i*)&result(k, j));  // 加载数据到AVX向量
//             __m256i sum_vec = _mm256_add_epi32(result_ik_vec, result_kj_vec); // 使用向量化指令进行计算
//             __m256i min_vec = _mm256_min_epi32(result_ij_vec, sum_vec);
//             _mm256_storeu_si256((__m256i*)&result(i, j), min_vec);// 将结果存回内存
//         }
//     }
// }  
// Graph Graph::apsp() {
//    Graph result(*this);
//    const int num_threads = 8;  // 获取可用的线程数
//    ThreadPool pool(num_threads);
//    const int chunk_size = vertex_num_ / num_threads;  // 每个线程负责的计算块大小
//    for (int k = 0; k < vertex_num_; ++k) {
//        std::vector<std::future<void>> futures;
//        for (int t = 0; t < num_threads; ++t) {
//             int start_idx = t * chunk_size;
//             int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
//             futures.emplace_back(pool.enqueue(apsp_t,std::ref(result),start_idx,end_idx,k,vertex_num_));
//        }
//        for (auto& future : futures) {
//            future.wait();
//        }
//    }
//    return result;
// }

/******************************************************************************************************************/


// class ThreadPool {
// public:
//     ThreadPool(size_t);
//     template<class F, class... Args>
//     auto enqueue(F&& f, Args&&... args) 
//         -> std::future<typename std::result_of<F(Args...)>::type>;
//     ~ThreadPool();
// private:
//     // need to keep track of threads so we can join them
//     std::vector< std::thread > workers;
//     // the task queue
//     std::queue< std::function<void()> > tasks;
//     // synchronization
//     std::mutex queue_mutex;
//     std::condition_variable condition;
//     bool stop;
// };
// // the constructor just launches some amount of workers
// inline ThreadPool::ThreadPool(size_t threads):stop(false)
// {
//     for(size_t i = 0;i<threads;++i)
//         workers.emplace_back(
//             [this]{
//                 for(;;){
//                     std::function<void()> task;{
//                         std::unique_lock<std::mutex> lock(this->queue_mutex);
//                         this->condition.wait(lock,
//                             [this]{ return this->stop || !this->tasks.empty(); });
//                         if(this->stop && this->tasks.empty())
//                             return;
//                         task = std::move(this->tasks.front());
//                         this->tasks.pop();
//                     }
//                     task();
//                 }
//             }
//         );
// }
// // add new work item to the pool
// template<class F, class... Args>
// auto ThreadPool::enqueue(F&& f, Args&&... args)-> std::future<typename std::result_of<F(Args...)>::type>
// {
//     using return_type = typename std::result_of<F(Args...)>::type;
//     auto task = std::make_shared< std::packaged_task<return_type()> >(
//             std::bind(std::forward<F>(f), std::forward<Args>(args)...)
//         );
//     std::future<return_type> res = task->get_future();
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         // don't allow enqueueing after stopping the pool
//         if(stop)
//             throw std::runtime_error("enqueue on stopped ThreadPool");
//         tasks.emplace([task](){ (*task)(); });
//     }
//     condition.notify_one();
//     return res;
// }
// // the destructor joins all threads
// inline ThreadPool::~ThreadPool()
// {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         stop = true;
//     }
//     condition.notify_all();
//     for(std::thread &worker: workers)
//         worker.join();
// }
// void apsp_t(Graph& result, int start_idx, int end_idx, int k, int vertex_num) {
//    for (int i = start_idx; i < end_idx; ++i) {
//         int result_ik = result(i, k);
//         __m256i result_ik_vec = _mm256_set1_epi32(result_ik);
//         for (int j = 0; j < vertex_num; j += 8) {
//             __m256i result_ij_vec = _mm256_loadu_si256((__m256i*)&result(i, j));
//             __m256i result_kj_vec = _mm256_loadu_si256((__m256i*)&result(k, j)); 
//             __m256i sum_vec = _mm256_add_epi32(result_ik_vec, result_kj_vec); 
//             __m256i min_vec = _mm256_min_epi32(result_ij_vec, sum_vec);
//             _mm256_storeu_si256((__m256i*)&result(i, j), min_vec);
//         }
//     }
// }  
// Graph Graph::apsp() {
//     Graph result(*this);
//     int num_threads = 8;
//     ThreadPool pool(4);
//     std::vector<std::future<int>> results;
//     const int chunk_size = vertex_num_ / num_threads;
//     for (int k = 0; k < vertex_num_; ++k) {
//         for (int t = 0; t < num_threads; ++t) {
//             int start_idx = t * chunk_size;
//             int end_idx = (t == num_threads - 1) ? vertex_num_ : start_idx + chunk_size;
//             results.emplace_back(
//                 pool.enqueue(apsp_t, std::ref(result), start_idx, end_idx, k, vertex_num_)
//             );
//         }
//     }
//     for(auto && r: results)
//         std::cout << r.get() << ' ';
//     std::cout << std::endl;
//     return result;
// }
//8.用GPU进行优化


