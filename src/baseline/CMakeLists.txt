add_library(apsp_baseline SHARED)
target_sources(apsp_baseline PRIVATE apsp.cc)
# target_compile_options(apsp_baseline PRIVATE -O3 -march=native -fopenmp -Wall -Wextra -Wpedantic -Werror)