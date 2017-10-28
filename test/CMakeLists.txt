cmake_minimum_required (VERSION 2.6)
project (p2psc_test)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-missing-braces -Wno-unused-function -std=c++14")

add_executable(p2psc_test
        test.cpp

        p2psc/connection_test.cpp
        p2psc/message_test.cpp
        p2psc/rsa_test.cpp)

target_link_libraries(p2psc_test
        p2psc
        boost_system
        boost_filesystem)
target_include_directories(p2psc_test PRIVATE
        .
        ../src)