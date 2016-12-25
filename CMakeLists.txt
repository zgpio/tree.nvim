cmake_minimum_required(VERSION 2.8.3)

project(neovim_client_cpp)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system coroutine context thread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    include
    ${Boost_INCLUDE_DIR}
)

add_executable(test 
    src/main.cpp
    src/socket.cpp
)

target_link_libraries(test ${Boost_LIBRARIES})

