cmake_minimum_required (VERSION 2.6)
project (p2psc_integration_test)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-missing-braces -Wno-unused-function -std=c++14")

add_executable(p2psc_integration_test
        src/util/client.h
        src/util/fake_mediator.h
        src/util/key_to_identifier_store.h

        test.cpp
        src/client_it.cpp
        src/util/client.cpp
        src/util/fake_mediator.cpp
        src/util/key_to_identifier_store.cpp
        include/util/peer_identifier.h)

target_link_libraries(p2psc_integration_test
        p2psc
        boost_system
        boost_filesystem)
target_include_directories(p2psc_integration_test PRIVATE
        .
        ../src)