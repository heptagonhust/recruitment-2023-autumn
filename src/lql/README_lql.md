# Task 0
## Floyd算法
Floyd算法用于求解无负权的环路途的最短路径
通过判断在i,j两个节点之间是否存在一个中间节点k使得其路径最短，若是更短则将i,j的路径更新为i->k->j。其本身是一个$O(n^3)$的算法，嵌套了三次for循环，其中存在大量的计算可以进行并行计算。
OpenMP的执行模型采用fork-join的形式，其中fork创建线程或者唤醒已有线程；join即多线程的会合。fork-join执行模型在刚开始执行的时候，只有一个称为“主线程”的运行线程存在。主线程在运行过程中，当遇到需要进行并行计算的时候，派生出线程来执行并行任务。在并行执行的时候，主线程和派生线程共同工作。在并行代码执行结束后，派生线程退出或者阻塞，不再工作，控制流程回到单独的主线程中。OpenMP线程：在OpenMP程序中用于完成计算任务的一个执行流的执行实体，可以是操作系统的线程也可以是操作系统上的进程。
### opt_example
在opt_example中使用的是下标追溯，减少多次调用函数访问同一个元素
## OpenMp的基本原理
OpenMP制导指令将C语言扩展为一个并行语言，但OpenMP本身不是一种独立的并行语言，而是为多处理器上编写并行程序而设计的、指导共享内存、多线程并行的编译制导指令和应用程序编程接口(API)，可在C/C++和Fortran中应用，并在串行代码中以编译器可识别的注释形式出现。
## 数据依赖
因为在这个过程中，两个节点中的路径长度是在实时更新的，比如1->2更新为1->3->2，记录的是最短路径的值，如果在并行计算中，由于没有顺序限制，可能存在我在考虑2作为中间节点，寻找1,4间的最短路径时，没有更新1->2的路径长度导致出错。
## 优化的循坏
由数据依赖我们可以考虑优化每个中间节点内部的循环，并且结合使用openmp中的barrier防止运算错误。
## 平衡线程负载及线程数的选择
在OpenMP中平衡线程负载可以对数据进行划分，每个线程处理不同的子集，但是在这种划分下，不同的线程分配到的任务依旧无法完美平衡。

OpenMP提供了解决的一些方法，OpenMP提供了schedule子句来实现任务的调度。

schedule子句：  schedule(type[, size])，

参数type是指调度的类型，主要用到的取值为static，dynamic，guided。static是默认值，将数据逐个依次分配。dynamic动态调度依赖于运行时的状态动态确定线程所执行的迭代，也就是线程执行完已经分配的任务后，会去领取还有的任务。由于线程启动和执行完的时间不确定，所以迭代被分配到哪个线程是无法事先知道的。当不使用size时，是将迭代逐个地分配到各个线程。当使用size 时，逐个分配size个迭代给各个线程。guide启发式调度，先分配给每个线程较多任务，任务完成后继续分配任务但是相对较少一些。

线程数的选择则需要依据处理器CPU来判断
## 疑问
集群上只能创造4个线程吗，我在lql_test中创造多个线程但是设定只有4个???
## Reference
[1]何亚茹,庞建民,徐金龙等.基于神威平台的Floyd并行算法的实现和优化[J].计算机科学,2021,48(06):34-40.  
[2]彭瑾,杨勇.基于OpenMP的Floyd并行算法研究[J].鞍山师范学院学报,2023,25(04):49-54.
[3]算法导论
# Task 1
采取的方式：pthread多线程加速/CUDA（貌似没有GPU？）
## pthread原理
### 线程与进程
进程：具有一定独立功能的程序关于某个数据结合上的依次运行活动，进程是系统进行资源分配和调度的一个独立单位。

线程：进程的实体，可以与同属于一个进程的其他线程共享资源，同一个进程中的多个线程可以共享该进程
### 线程初始化
```c++
#include<pthread.h>
int pthread_once(pthread_once_t *once_control,void(*init_routine)(void));   //once_control 控制变量，仅执行一次初始化，init_routine 初始化函数
```
### 使用pthread_create创建进程
```c++
#include <pthread.h>
int pthread_create(
    pthread_t *restrict tid                 //指向线程标识符的指针，句柄，线程号，用于管理线程
    const pthread_attr_t *restrict attr     //设置线程属性，默认NULL
    void *(*strat_routine)(void *)          //线程的入口函数strat_routine,线程从这个函数开始独立运行，返回值void*，入口函数有一个返回值，通过pthread_join()获取
    void *restrict arg                      //入口函数参数*arg:start_routine
)
```
### 线程的终止
```c++
#include<pthread.h>
// tid 线程号  status 数据指针
int pthread_detach(pthread_t tid);              //从状态实现线程分离，将回收工作交给系统，得不到返回值
int pthread_cancel(pthread_t tid);              //发出停止指令，但是不意味着终止，可以被另一个线程取消掉
int sched_yield(void);                          //停止当前线程，执行优先度更高的线程
int pthread_join(pthread_t tid,void** status);  //主线程阻塞等待子线程结束，合并线程，回收子线程资源，防止资源泄露
void pthread_exit(void* status);                //释放所有线程特定数据绑定
```
## pthread优化思路
- 设定线程数  
- 创立线程
  - 将任务按照合理方式分配，将数据分块处理，然后分配到不同的线程
  - 编写入口函数，设定合适的入口函数参数，我采用的是设定结构数组，结构包含原类中的result（Graph*），内存循环开始的下标，start和end
  - 在入口函数中完成比较计算
- 合并线程，回收子线程资源
- 返回result
### 优化结果
```
pthread on 128      ***Failed    0.07 sec
pthread on 512      ***Failed    0.30 sec
pthread on 1024     ***Failed    1.03 sec
pthread on 4096     ***Exception: SegFault 19.61 sec
```
## 问题
虽然速度很快但是与结果不匹配，返回failed（悲）  
- debug1：怀疑是没有加锁，但是理论上这里应该不需要加锁，不会出现同时访问修改的现象？
- debug2：怀疑是我对任务的分配有问题，但是依旧failed
- debug3：怀疑是结构体传参时并没有改变原result中的数据，尝试修改结构体传参无果，考虑到不能修改其他文件，无法在Graph中重新定义  

最终修改无果，可以看到数据运算变快了，但是对数据的处理却发生了问题（）。
## thread 优化
在thread中使用的是线程库\<thread>
### 创建线程
```c++
#include<thread>
#include<iostream>
//使用std::thread类创建
std::thread()                           //默认的构造函数，创建一个新的thread执行对象
std::thread newThread(func,args,...);   //创建线程的同时调用函数func，args代表传的参数，创建线程即执行
/*
传递参数时可以使用std::ref()和std::cref()
std::ref()包装引用传递的值为右值
std::cref()包装按照const引用的值
*/
```
### 其他成员函数
```c++
std::newThread::get_id();         //获取线程id
std::newThread::joinable();       //检查线程是否可以被join
std::newThread::join();           //join线程，等待线程结束，回收资源
std::newThread::detach();         //将线程分离，等待系统回收资源
```
### 锁
```c++
#include <thread>
#include <iostream>
std::mutex mtx;                   //定义互斥量
mtx.lock();                       //上锁，阻塞线程直到解锁，如果已经被锁住了，则会产生死锁
mtx.unlock();                     //解锁，释放
mtx.try_lock();                   //尝试上锁，如果未被上锁，则上锁返回true，如果已被锁死，则返回false
```
### 原子
```c++
#include <thread>
#include <iostream>
std::atomic;                      //原子操作，最小的不可并行的操作，减少上锁和解锁的消耗
std::atomic<int> num;             //或者std::atomic_int
```
### 优化结果
使用thread优化过程中则得到了想要的结果
```
thread on 128       Passed    0.12 sec
thread on 512       Passed    2.12 sec
thread on 1024      Passed   14.84 sec
thread on 4096      Passed  592.46 sec
```
发现效果得到了优化，但是并没有预期的那么好，我们尝试继续优化，减少他的函数调用，仿照opt_example，得到如下结果
```
thread_opt on 128   Passed    0.09 sec
thread_opt on 512   Passed    1.02 sec
thread_opt on 1024  Passed    5.27 sec
thread_opt on 4096  Passed  211.91 sec
```
优化又提高了一倍