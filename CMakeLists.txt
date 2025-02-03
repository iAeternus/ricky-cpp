# ===================================================================
# 主项目CMake配置
# ===================================================================
cmake_minimum_required(VERSION 3.12)
project(RICKY_CPP VERSION 1.0
    LANGUAGES CXX
    DESCRIPTION "My C++ Utilities"
)

# ----------------------------------------
# 编译器标准配置
# ----------------------------------------
set(CMAKE_CXX_STANDARD 23)            # 强制使用C++23标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # 必须支持C++23
set(CMAKE_CXX_EXTENSIONS OFF)         # 禁用编译器扩展

# ----------------------------------------
# 并行编译配置（自动检测CPU核心数）
# ----------------------------------------
include(ProcessorCount)
ProcessorCount(NUM_JOBS)
if(NUM_JOBS EQUAL 0)
    set(NUM_JOBS 4)  # 核心数检测失败时回退到4线程
endif()
message(STATUS "[MAIN] Parallel jobs: ${NUM_JOBS}")

# ----------------------------------------
# 预编译头文件（PCH）配置
# ----------------------------------------
set(PCH_HEADER "${CMAKE_SOURCE_DIR}/include/common/PCH.h")
if(EXISTS ${PCH_HEADER})
    message(STATUS "[MAIN] Enabling precompiled headers")
    # 为可执行文件设置PCH（Require CMake 3.16+）
    target_precompile_headers(RICKY_CPP PRIVATE ${PCH_HEADER})
endif()

# ----------------------------------------
# 源码管理
# ----------------------------------------
file(GLOB_RECURSE SRC_FILES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cxx"
)

# 添加可执行文件
add_executable(RICKY_CPP ${SRC_FILES})

# ----------------------------------------
# include目录配置
# ----------------------------------------
target_include_directories(RICKY_CPP PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/io
    ${CMAKE_SOURCE_DIR}/include/util
    ${CMAKE_SOURCE_DIR}/include/test
    ${CMAKE_SOURCE_DIR}/include/async
    ${CMAKE_SOURCE_DIR}/include/fs
    ${CMAKE_SOURCE_DIR}/include/math
    ${CMAKE_SOURCE_DIR}/include/json
    ${CMAKE_SOURCE_DIR}/include/graph
    ${CMAKE_SOURCE_DIR}/include/coro
)

# ----------------------------------------
# 编译器优化
# ----------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC优化配置
    target_compile_options(RICKY_CPP PRIVATE
        -Wall -Wextra -Wpedantic  # 严格警告
        -O2                       # 发布模式优化
        -ffunction-sections       # 函数分段（减少二进制体积）
        -fdata-sections           # 数据分段
    )
    # 链接器优化（gold/lld）
    include(CheckLinkerFlag)
    check_linker_flag(CXX "-fuse-ld=gold" LINKER_SUPPORTS_GOLD)
    check_linker_flag(CXX "-fuse-ld=lld" LINKER_SUPPORTS_LLD)
    if(LINKER_SUPPORTS_GOLD)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--gc-sections")
        message(STATUS "[MAIN] Using GNU gold linker")
    elseif(LINKER_SUPPORTS_LLD)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -Wl,--gc-sections")
        message(STATUS "[MAIN] Using LLVM lld linker")
    endif()

elseif(MSVC)
    # MSVC优化配置
    target_compile_options(RICKY_CPP PRIVATE
        /W4       # 最高警告等级
        /WX       # 视警告为错误
        /MP       # 多进程编译
        /GL       # 全程序优化
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")  # 链接时代码生成
endif()

# ----------------------------------------
# 输出目录配置
# ----------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # 可执行文件

# ----------------------------------------
# 测试配置
# ----------------------------------------
add_subdirectory(tests)
enable_testing()