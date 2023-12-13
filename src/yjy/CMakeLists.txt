add_library(apsp_yjy SHARED)
 
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread -O0 -msse4.1")
target_sources(apsp_yjy PRIVATE apsp_yjy.cc)
# target_compile_options(apsp_baseline PRIVATE -O3 -march=native -fopenmp -Wall -Wextra -Wpedantic -Werror)