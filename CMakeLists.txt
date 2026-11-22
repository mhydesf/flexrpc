# test
cmake_minimum_required(VERSION 3.26.0)
project(flexRPC)

set(CMAKE_CXX_STANDARD 23)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE
        "${CMAKE_SOURCE_DIR}/cpm_cache"
        CACHE STRING "CPM Cache Dir")
endif()

include(CPM)
include(protobuf)

add_subdirectory(proto flexrpc_proto)

add_library(${PROJECT_NAME}-lib INTERFACE)
target_include_directories(${PROJECT_NAME}-lib INTERFACE include)
target_link_libraries(${PROJECT_NAME}-lib INTERFACE flexrpc-proto)

add_executable(client src/client.cpp)
target_link_libraries(client PRIVATE ${PROJECT_NAME}-lib)

add_executable(server src/server.cpp)
target_link_libraries(server PRIVATE ${PROJECT_NAME}-lib)
