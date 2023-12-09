add_library(apsp_tr SHARED)
target_sources(apsp_tr PRIVATE apsp_tr.cc)
# target_compile_options(apsp_baseline PRIVATE -O3 -march=native -fopenmp -Wall -Wextra -Wpedantic -Werror)