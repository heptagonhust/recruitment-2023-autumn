add_library(apsp_gzbz SHARED)
target_sources(apsp_gzbz PRIVATE apsp_gzbz.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O0")

target_link_libraries(apsp_gzbz -pthread)

