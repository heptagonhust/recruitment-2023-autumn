add_library(apsp_lql SHARED)
target_sources(apsp_lql PRIVATE apsp_lql.cc)
target_link_libraries(apsp_lql pthread)
# target_compile_options(apsp_baseline PRIVATE -O0 -lpthread -march=native  -Wall -Wextra -Wpedantic -Werror)