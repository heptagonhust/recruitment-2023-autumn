# APSP

你需要实现对 Floyd-Warshall 算法的优化。如果你对图或全源最短路仍不太了解，可以通过书籍、博客与其他课程等多种途径进行学习，或参考reference中给出的网址。以下是 Floyd-Warshall 算法的伪代码：

```c
// 设图 G = (V, E) 为赋权有向图
// 输入图的大小 |V| = n
// 输入邻接矩阵 D[n][n]，D[x][y] 表示点 x 到点 y 的距离
for (int k = 0; k < n; k++)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            D[i][j] = min(D[i][j], D[i][k] + D[k][j]);
```

### 数据约定

- 保证图中每个点到自己的距离为0
- 图为完全图，保证邻接矩阵中不存在无穷大的项
- 保证图中任意两点之间的距离小于等于INT_MAX / n

### 优化指引

- 要对代码进行优化，你不需要且不可以更改现有代码目录下的任何文件
- 你需要在 `src/` 目录下建立自己的子目录，该子目录应该与baseline目录同层级，此处假设该子目录名为 `your_name`
- 你需要在 `src/your_name`下新建 `CMakeLists.txt` ，在其中进行你的目录及编译选项等配置，并将最终的目标文件设置为dynamic library类型，且将目标命名为 `apsp_${your_name}`
- 完成以上配置后，可直接在 `APSP/` 目录下执行`cmake -B build && cmake --build build`，即可生成用于测试的可执行文件，文件位于 `build/test/` ，名称为 `apsp_test_${your_name}`
- 你可以直接执行 `ctest --test-dir build`，即可对你的实现进行测试
- tips
    - ctest返回的时间并非你的apsp计算消耗的时间，而是整个测试程序执行的时间。测试程序内对apsp耗时进行了单独测试，但默认不会打印。若需打印准确的执行时间和其他程序输出，你需要在ctest后添加-V参数。
    - 注意子目录命名和编译目标命名的对应，这会直接影响到编译与测试的结果

### 测试数据

- 测试数据位于 `/tmp/dataset/APSP`下，分别为大小128，512，1024，4096节点的图的邻接矩阵
- baseline在cpu上单核执行，执行时间约为：
    - 128: 0.04 s
    - 512: 2.5 s
    - 1024: 18 s
    - 4096: 1160 s

### NOTICE
- 由于考察的是这种特定的计算模式，不允许进行算法优化

### Reference

[最短路 - OI Wiki](https://oi-wiki.org/graph/shortest-path/#floyd-算法)