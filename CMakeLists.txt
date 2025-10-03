cmake_minimum_required(VERSION 3.10)
project(llh2 LANGUAGES C)
project(llh2_tests LANGUAGES CXX)

# set(
#     llh2_test_sources
#     tests/main.cpp
#     tests/test_llh2.cpp
#     )
set(
    llh2_sources
    src/api.c
    src/llh2.c
    native/http2.c
)

add_library(llh2 STATIC ${llh2_sources})
target_include_directories(
    llh2 
    PUBLIC include
)
