add_library(apsp_opt_example SHARED)
target_sources(apsp_opt_example PRIVATE apsp_opt.cc)
target_compile_options(apsp_opt_example PRIVATE -O0 -march=native -Wall -Wextra -Wpedantic -Werror)