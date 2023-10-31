# 23-autumn-recruiment
## 背景：有向图全源最短路
给定一个有向图$G = <V,E>$，求任意一对顶点之间的最短路，即全源最短路（All Pairs Shortest Path, APSP）问题。
## 算法：FLoyd-Wallshall算法
FLoyd-Wallshall算法是一种求解无负权环路图的APSP问题的动态规划算法。其基本原理是递推地求出任意一对点之间仅经过$\{1,2,...,k\} \subset E$中的顶点的最短路，直至$k = n$。

在接下来的任务中我们主要关心算法的实现和计算过程，如果需要进一步了解APSP问题，请参考references中的网站或相关书籍。

## 项目结构
我们为您提供基础代码文件，文件结构如下：
```
.
└── recruitment-2023-autumn
    ├── CMakeLists.txt
    ├── README.md
    ├── cmake
    │   └── utils.cmake
    ├── include // 图的定义 & 计时器
    │   ├── graph.hh
    │   └── timer.hh
    ├── src     // 算法代码文件
    │   ├── baseline
    │   │   ├── CMakeLists.txt
    │   │   └── apsp.cc
    │   └── graph.cc
    ├── test
    │   ├── CMakeLists.txt
    │   ├── apsp_test.cc
    │   └── tests.cmake
    └── utils
        ├── CMakeLists.txt
        └── generator.cc
```

## 任务简述
在这个任务中，您需要完成对 Floyd-Warshall 算法的性能优化，以下是 Floyd-Warshall 算法的伪代码：

```c++
// 设图 G = (V, E) 为赋权有向图，V为点集，E为边集
// 输入图的大小 |V| = n
// 输入邻接矩阵 D[n][n]，D[x][y] 表示点 x 到点 y 的有向边的长度
for (int k = 0; k < n; k++)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            D[i][j] = min(D[i][j], D[i][k] + D[k][j]);
```

## 任务具体描述
### TASK0 并行初体验

如果您还不太熟悉性能优化的相关技巧，在这个任务中，您可以通过使用`OpenMP`体验简单的线程级并行。

- 完成这个任务需要您掌握如下内容：
  - `Cmake`添加目标文件和编译选项
  - 在C++中使用`OpenMP`编程

- 您应该思考以下问题：
  - 计算过程有什么样的数据依赖？
  - 应该将哪一层循环并行化？
  - 如何平衡线程的负载？
  - 线程数应该设置为多少？

- 注：不计入最终结果，但需要呈现在最终的成果展示中。

### TASK1 主要任务

要求对Floyd-Warshall算法进行优化。**不允许**使用TASK0中的`OpenMP`接口。
您可以选择：
- 将任务分配到CPU的多个处理器单元上进行计算
- 使用向量计算指令进行数据级并行
- 选取合适的数据分块方式
- 在GPU上编程实现该算法
- 更多有关高性能计算的学习方向与资料，可翻阅 references 中的[七边形HPC-roadmap](https://heptagonhust.github.io/HPC-roadmap/)
### 数据约定

- 保证图中每个点到自己的距离为0
- 图为完全图，保证邻接矩阵中不存在无穷大的项
- 保证图中不存在负环
- 保证图中任意两点之间的距离小于等于 `INT_MAX / n`

### 操作指引
- 对于每个通过面试的选手，我们会提供超算队 CPU 集群的访问权限，选手**应当**在集群上对自己编写的程序进行测试。

- 要对代码进行优化，您不需要且不可以更改现有代码目录下的任何文件
- 您需要在 `src/` 目录下建立自己的子目录，该子目录应该与`baseline`目录同层级，该子目录名应为 `your_name`（姓名首字母缩写），您可以对您的子目录中的内容进行**任何**修改
- 您需要在 `src/your_name` 下新建 `CMakeLists.txt` ，在其中进行您的目录及编译选项等配置，并将最终的目标文件设置为dynamic library类型，且将目标命名为 `apsp_${your_name}`。注意：所有的选项配置必须设置为仅对自己的target生效，**不可污染其他target的配置**
- 完成以上配置后，可直接在 `APSP/` 目录下执行`cmake -B build && cmake --build build`，（ `-B` 指定构建文件夹， `--build` 选项执行构建）即可生成用于测试的可执行文件，文件位于 `build/test/` ，名称为 `apsp_test_${your_name}`
- 您可以直接执行 `ctest --test-dir build`，即可对您的实现进行测试
- `ctest`返回的时间并非您的apsp计算消耗的时间，而是整个`ctest`测试程序执行的时间。测试程序内对apsp算法耗时进行了单独测试，但默认不会打印。**若需打印算法真实的执行时间和其他程序输出，您需要在`ctest`后添加`-V`参数，测试时以此时间为准**。

- 可以使用 `-E <target_name>` 跳过 对 `src/` 下 `CMakeLists.txt` 中编译目标 `apsp_${target_name}` 的测试，例如：
要跳过`src/baseline`目录中配置的编译目标 `apsp_baseline` 的测试，应当使用如下命令：
  ```shell
  ctest --test-dir build -E baseline
  ```
- 注意子目录命名和编译目标命名的对应，这会直接影响到编译与测试的结果
### 测试数据

- 测试数据位于 `/tmp/dataset/APSP` 下，分别为大小128，512，1024，4096节点的图的邻接矩阵
- 我们已经为您测试了 `baseline` 在集群CPU单核上的真实执行时间：
    ```shell
    Baseline 128: 约 45 ms
    Baseline 512: 约 2.3 s
    Baseline 1024: 约 18.3 s
    Baseline 4096: 约 1160.0 s
    ```

### 提交方式

您应当**fork**此代码仓库，并以此为基础进行开发。

您需要在XX月XX日XX:XX:XX之前将自己的开发仓库以pull request的方式提交到本仓库下并注明院系和姓名，我们会收集你的代码文件并测试。

此外，在解题完毕后您需要制作一份ppt并作优化成果的展示，内容如下：

1. **每一步**优化的思路、过程和效果（举例：使用了 xxx 优化，相对于原代码，速度提升了 114.514 倍）
2. 如果对baseline程序进行 profile，可以分析性能瓶颈
3. 您在解题过程中所参考的资料（如有使用人工智能工具，请注明）
4. 在解题过程中，遇到的有意思的事情，或者是让您印象深刻的 bug（可选）

### NOTICE

- 由于考察的是这种特定的计算模式，**不允许进行算法优化**。
- 对题目和提交方式对任何疑问可以随时在群组中咨询我们。

## References

- [最短路 - OI Wiki](https://oi-wiki.org/graph/shortest-path/#floyd-算法)

- [七边形HPC-roadmap](https://heptagonhust.github.io/HPC-roadmap/)

- [OpenMP](https://www.openmp.org/resources/refguides/)

- [SIMD入门](https://zhuanlan.zhihu.com/p/583326378)， [内建函数参考手册](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#)
