# ===================================================================
# 主项目配置 - Ricky C++ Utilities
# ===================================================================
cmake_minimum_required(VERSION 3.20)
project(RICKY_CPP VERSION 1.0 LANGUAGES CXX DESCRIPTION "My C++ Utilities")

# -------------------------------------------------------------------
# 编译器标准配置
# -------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
    add_compile_options(-Wno-format)
endif ()

# -------------------------------------------------------------------
# 构建系统优化
# -------------------------------------------------------------------
if (NOT CMAKE_GENERATOR)
    find_program(NINJA_EXE ninja)
    if (NINJA_EXE)
        message(STATUS "[MAIN] Ninja detected. Consider using -G Ninja for faster builds.")
    endif ()
endif ()

# 设置并行编译
include(ProcessorCount)
ProcessorCount(NUM_JOBS)
set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_JOBS})
message(STATUS "[MAIN] Parallel jobs: ${NUM_JOBS}")

# -------------------------------------------------------------------
# 子目标定义（拆分后的多目标在 src/CMakeLists.txt 中定义）
# -------------------------------------------------------------------
add_subdirectory(src)

# -------------------------------------------------------------------
# 输出目录与子项目
# -------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加测试和示例目录
option(RICKY_BUILD_TESTS "Build tests" ON)
option(RICKY_BUILD_EXAMPLES "Build examples" ON)

if (RICKY_BUILD_TESTS)
    add_subdirectory(tests)
    enable_testing()
endif ()

if (RICKY_BUILD_EXAMPLES AND EXISTS ${CMAKE_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/include/CMakeLists.txt)
    add_subdirectory(include)
endif ()

# -------------------------------------------------------------------
# 其他配置
# -------------------------------------------------------------------
option(STRICT_FORMAT_CHECK "Enable strict format string checking" OFF)
