cmake_minimum_required(VERSION 3.1.3)

project(flow_graph_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}
    main.cpp
    simplegraph.cpp
    simplesequencegraph.cpp
    limitgraph.cpp
    limitsequencegraph.cpp
    parallelgraph.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(TBB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/tbb)
include(${TBB_ROOT}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${TBB_ROOT} CONFIG_DIR TBB_DIR MAKE_ARGS stdver=c++11 tbb_cpf=1)
find_package(TBB REQUIRED tbb_preview)

target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS})

find_package(Threads REQUIRED)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
