cmake_minimum_required(VERSION 2.8.12)

project(flow_graph_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}
    main.cpp
    simplegraph.cpp
    simplesequencegraph.cpp
    limitgraph.cpp
)

set(TBB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/tbb)
include(${TBB_ROOT}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${TBB_ROOT} CONFIG_DIR TBB_DIR MAKE_ARGS stdver=c++11 tbb_cpf=1)
find_package(TBB REQUIRED tbb_preview)

target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS})
