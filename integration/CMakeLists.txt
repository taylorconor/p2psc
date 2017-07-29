cmake_minimum_required (VERSION 2.6)
project (p2psc_integration_test)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-missing-braces -std=c++14")

add_executable(p2psc_integration_test
        include/util/client.h

        test.cpp
        src/util/client.cpp
        src/client_it.cpp)

target_link_libraries(p2psc_integration_test
        p2psc)
target_include_directories(p2psc_integration_test PRIVATE
        .
        ../src)