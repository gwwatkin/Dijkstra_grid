cmake_minimum_required(VERSION 3.17)
project(Dijkstra_grid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wfatal-errors -Wall -pedantic")

find_package(Boost)



add_executable(dijkstraX dijkstraX.cpp)
add_executable(jhonsonX jhonsonX.cpp)

add_executable(grid_graph_example GridWithRoughBoundaries.cpp grid_graph_example.cpp)

add_executable(GridWithRoughBoundaries_test GridWithRoughBoundaries.cpp GridWithRoughBoundaries_test.cpp)

add_executable(makeGrid GridWithRoughBoundaries.cpp makeGrid.cpp)
