cmake_minimum_required(VERSION 2.8.3)

project(neovim_client_cpp)

find_package(Boost REQUIRED COMPONENTS system)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ${Boost_INCLUDE_DIR}
)

add_executable(test src/main.cpp)

target_link_libraries(test ${Boost_LIBRARIES})
