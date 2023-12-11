add_library(apsp_tr SHARED)
target_sources(apsp_tr PRIVATE apsp_tr.cc)
# target_compile_options(apsp_baseline PRIVATE -O3 -march=native -fopenmp -Wall -Wextra -Wpedantic -Werror)

# 设置编译器优化等级
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Threads REQUIRED)
target_link_libraries(apsp_tr PRIVATE Threads::Threads)
