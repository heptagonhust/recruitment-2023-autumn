cmake_minimum_required(VERSION 3.25)
include("cmake/utils.cmake")

project(APSP LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

include_directories(include)

add_library(graph STATIC)
aux_source_directory(src SOURCES)
target_sources(graph PRIVATE ${SOURCES})

set(SrcDir ${PROJECT_SOURCE_DIR}/src)
getSubDir(SubDirs ${SrcDir})
foreach(SubDir ${SubDirs})
    add_subdirectory(${SrcDir}/${SubDir})
endforeach()

add_subdirectory(utils)
add_subdirectory(test)