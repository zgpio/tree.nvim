cmake_minimum_required(VERSION 2.8.3)

project(neovim_client_cpp)

find_package(Boost 1.55.0 REQUIRED COMPONENTS filesystem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pthread")

set(CMAKE_CXX_FLAGS "-DMSGPACK_USE_BOOST ${CMAKE_CXX_FLAGS}") #TODO: import every flags directly from msgpack-c
# to generate a compile_commands.json usable by ycm and other tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# msgpack
option(USE_SYSTEM_MSGPACK "Use system msgpack libraries " OFF)
if(USE_SYSTEM_MSGPACK)
    find_package(Msgpack REQUIRED)
else()
    add_subdirectory(external)
endif()

include_directories(
    include
    gen
    ${Boost_INCLUDE_DIR}
    ${MSGPACK_INCLUDE_DIRS}
)

add_definitions( -DBOOST_ALL_NO_LIB )
add_executable(tree
    src/main.cpp
    src/app/tree.cpp
    src/app/app.cpp
    src/app/column.cpp
    src/app/wcwidth.cpp
    src/socket.cpp
    src/util.cpp
    gen/nvim.cpp
)

target_link_libraries(tree ${Boost_LIBRARIES})
install(DIRECTORY src/app/viml/ DESTINATION ${CMAKE_INSTALL_PREFIX})
# install(TARGETS tree DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS tree DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

find_package(PythonInterp)
if (PYTHONINTERP_FOUND)
    add_custom_target(auto_gen_api
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/gen_api.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Autogen nvim interface"
    )
endif()

if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
    add_definitions(-D Q_OS_WIN)
endif()
# add_dependencies(tree auto_gen_api)
