cmake_minimum_required (VERSION 2.6)
project (p2psc)

target_include_directories(p2psc
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources(p2psc
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/base64.cpp)