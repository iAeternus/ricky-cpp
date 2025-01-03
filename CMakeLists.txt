cmake_minimum_required(VERSION 3.10)

project(RICKY_CPP VERSION 1.0 LANGUAGES CXX DESCRIPTION "My cpp wheels")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

aux_source_directory("${CMAKE_SOURCE_DIR}/src" SRC_DIR)

add_executable(RICKY_CPP 
    ${SRC_DIR} 
)

target_include_directories(RICKY_CPP PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/io"
    "${CMAKE_SOURCE_DIR}/include/util"
    "${CMAKE_SOURCE_DIR}/include/test"
    "${CMAKE_SOURCE_DIR}/include/async"
    "${CMAKE_SOURCE_DIR}/include/fs"
    "${CMAKE_SOURCE_DIR}/include/math"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
endif()

add_subdirectory(tests)
enable_testing()