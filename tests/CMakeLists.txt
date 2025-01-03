cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_TEST_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/tests/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_TEST_BINARY_DIR})

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" TESTS_DIR)

add_executable(RICKY_CPP_TESTS 
    ${TESTS_DIR}
)

target_include_directories(RICKY_CPP_TESTS PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/io"
    "${CMAKE_SOURCE_DIR}/include/util"
    "${CMAKE_SOURCE_DIR}/include/test"
    "${CMAKE_SOURCE_DIR}/include/async"
    "${CMAKE_SOURCE_DIR}/include/fs"
    "${CMAKE_SOURCE_DIR}/include/math"
)

add_test(NAME RICKY_CPP_TESTS COMMAND $<TARGET_FILE:RICKY_CPP_TESTS>)