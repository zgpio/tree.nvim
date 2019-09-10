cmake_minimum_required(VERSION 2.8.3)

project(neovim_client_cpp)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system coroutine context thread filesystem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(MSGPACK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/msgpack-c)

#add_subdirectory(${MSGPACK_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "-DMSGPACK_USE_BOOST ${CMAKE_CXX_FLAGS}") #TODO: import every flags directly from msgpack-c

include_directories(
    include
    gen
    ${Boost_INCLUDE_DIR}
    ${MSGPACK_SOURCE_DIR}/include
)

add_executable(test 
    src/main.cpp
    src/socket.cpp
    gen/nvim.cpp
)

target_link_libraries(test ${Boost_LIBRARIES})

find_package(PythonInterp)
if (PYTHONINTERP_FOUND)
    add_custom_target(auto_gen_api
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/gen_api.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Autogen nvim interface"
    )
endif()

add_dependencies(test auto_gen_api)
