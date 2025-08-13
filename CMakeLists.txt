## ===================================================================
## 主项目CMake配置
## ===================================================================
#cmake_minimum_required(VERSION 3.20)
#project(RICKY_CPP VERSION 1.0
#        LANGUAGES CXX
#        DESCRIPTION "My C++ Utilities"
#)
#
## ----------------------------------------
## 编译器标准配置
## ----------------------------------------
#set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
## ----------------------------------------
## 构建系统优化 (优先使用Ninja)
## ----------------------------------------
#if (NOT CMAKE_GENERATOR)
#    find_program(NINJA_EXE ninja)
#    if (NINJA_EXE)
#        set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "Build system generator")
#        message(STATUS "[MAIN] Using Ninja build system")
#    endif ()
#endif ()
#
## ----------------------------------------
## 并行编译配置
## ----------------------------------------
#if (NOT CMAKE_GENERATOR MATCHES "Ninja")
#    include(ProcessorCount)
#    ProcessorCount(NUM_JOBS)
#    if (NUM_JOBS EQUAL 0)
#        set(NUM_JOBS 4)
#    endif ()
#    set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_JOBS} CACHE STRING "Parallel build jobs")
#    message(STATUS "[MAIN] Parallel jobs: ${NUM_JOBS}")
#endif ()
#
## ----------------------------------------
## 源码管理
## ----------------------------------------
#file(GLOB_RECURSE SRC_FILES
#        "${CMAKE_SOURCE_DIR}/src/*.cpp"
#        "${CMAKE_SOURCE_DIR}/src/*.cxx"
#)
#
#add_executable(RICKY_CPP ${SRC_FILES})
#
## 延迟设置PCH（必须在目标创建后）
#if (ENABLE_PCH AND EXISTS ${PCH_HEADER})
#    message(STATUS "[MAIN] Enabling precompiled headers")
#    target_precompile_headers(RICKY_CPP PRIVATE ${PCH_HEADER})
#endif ()
#
## ----------------------------------------
## include目录配置
## ----------------------------------------
#target_include_directories(RICKY_CPP PRIVATE
#        ${CMAKE_SOURCE_DIR}/include
#        ${CMAKE_SOURCE_DIR}/include/io
#        ${CMAKE_SOURCE_DIR}/include/util
#        ${CMAKE_SOURCE_DIR}/include/test
#        ${CMAKE_SOURCE_DIR}/include/async
#        ${CMAKE_SOURCE_DIR}/include/fs
#        ${CMAKE_SOURCE_DIR}/include/math
#        ${CMAKE_SOURCE_DIR}/include/graph
#        ${CMAKE_SOURCE_DIR}/include/coro
#        ${CMAKE_SOURCE_DIR}/include/ai
#        ${CMAKE_SOURCE_DIR}/include/num
#        ${CMAKE_SOURCE_DIR}/include/mem
#        ${CMAKE_SOURCE_DIR}/include/net
#)
#
## 平台特定链接库
#if (WIN32)
#    target_link_libraries(RICKY_CPP PRIVATE ws2_32)
#elseif (UNIX)
#    target_link_libraries(RICKY_CPP PRIVATE pthread)
#endif ()
#
## ----------------------------------------
## 编译器优化与警告
## ----------------------------------------
## 公共编译选项
#target_compile_options(RICKY_CPP PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:
#        /W4 /WX
#        /MP           # 多进程编译
#        /permissive-  # 标准一致性模式
#        >
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
#        -Wall -Wextra -Wpedantic
#        -Wno-unused-parameter  # 忽略未使用的参数警告
#        >
#)
#
## 配置特定优化
#target_compile_options(RICKY_CPP PRIVATE
#        $<$<CONFIG:RELEASE>:
#        $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3 -flto>
#        >
#        $<$<CONFIG:DEBUG>:
#        $<$<CXX_COMPILER_ID:MSVC>:/Zi>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-g3>
#        >
#)
#
## 链接器优化
#target_link_options(RICKY_CPP PRIVATE
#        $<$<CONFIG:RELEASE>:
#        $<$<CXX_COMPILER_ID:MSVC>:/LTCG>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-flto>
#        >
#)
#
## GNU/Clang额外优化
#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(RICKY_CPP PRIVATE
#            -ffunction-sections
#            -fdata-sections
#    )
#    target_link_options(RICKY_CPP PRIVATE
#            -Wl,--gc-sections
#            $<$<CONFIG:RELEASE>:-Wl,--strip-all>
#    )
#
#    # 链接器选择
#    include(CheckLinkerFlag)
#    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#        set(LINKER_OPTIONS "-fuse-ld=lld" CACHE STRING "Linker selection")
#    else ()
#        set(LINKER_OPTIONS "-fuse-ld=gold" CACHE STRING "Linker selection")
#    endif ()
#
#    check_linker_flag(CXX ${LINKER_OPTIONS} LINKER_SUPPORTS_OPTION)
#    if (LINKER_SUPPORTS_OPTION)
#        target_link_options(RICKY_CPP PRIVATE ${LINKER_OPTIONS})
#        message(STATUS "[MAIN] Using ${LINKER_OPTIONS} linker")
#    endif ()
#endif ()
#
## ----------------------------------------
## 输出目录配置
## ----------------------------------------
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#
## ----------------------------------------
## 测试配置
## ----------------------------------------
#add_subdirectory(tests)
#enable_testing()

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

# -------------------------------------------------------------------
# 构建系统优化
# -------------------------------------------------------------------
if (NOT CMAKE_GENERATOR AND NINJA_EXE)
    set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "Build system generator")
    message(STATUS "[MAIN] Using Ninja build system")
endif ()

# 设置并行编译
include(ProcessorCount)
ProcessorCount(NUM_JOBS)
set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_JOBS})
message(STATUS "[MAIN] Parallel jobs: ${NUM_JOBS}")

# -------------------------------------------------------------------
# 头文件目录配置
# -------------------------------------------------------------------
# 递归获取所有头文件目录
file(GLOB_RECURSE HEADER_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/include/*")
foreach (dir ${HEADER_DIRS})
    if (IS_DIRECTORY ${dir})
        list(APPEND INCLUDE_DIRS ${dir})
    endif ()
endforeach ()
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# -------------------------------------------------------------------
# 可执行目标配置
# -------------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.cxx")
add_executable(RICKY_CPP ${SRC_FILES})

# 包含目录
target_include_directories(RICKY_CPP PRIVATE ${INCLUDE_DIRS})

# 平台特定库
if (WIN32)
    target_link_libraries(RICKY_CPP PRIVATE ws2_32)
elseif (UNIX)
    target_link_libraries(RICKY_CPP PRIVATE pthread)
endif ()

# -------------------------------------------------------------------
# 编译器与链接器配置
# -------------------------------------------------------------------
# 公共编译选项
target_compile_options(RICKY_CPP PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP /permissive->
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
)

# 配置特定优化
target_compile_options(RICKY_CPP PRIVATE
        $<$<CONFIG:RELEASE>:
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3 -flto>
        >
        $<$<CONFIG:DEBUG>:
        $<$<CXX_COMPILER_ID:MSVC>:/Zi>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-g3>
        >
)

# 链接器优化
target_link_options(RICKY_CPP PRIVATE
        $<$<CONFIG:RELEASE>:
        $<$<CXX_COMPILER_ID:MSVC>:/LTCG>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-flto>
        >
)

# GNU/Clang特定优化
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RICKY_CPP PRIVATE
            -ffunction-sections -fdata-sections -Wa,-mbig-obj
    )
    target_link_options(RICKY_CPP PRIVATE
            -Wl,--gc-sections $<$<CONFIG:RELEASE>:-Wl,--strip-all>
    )

    # 链接器选择
    set(LINKER_OPTIONS $<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>$<$<CXX_COMPILER_ID:GNU>:-fuse-ld=gold>)
    include(CheckLinkerFlag)
    check_linker_flag(CXX ${LINKER_OPTIONS} LINKER_SUPPORTS_OPTION)
    if (LINKER_SUPPORTS_OPTION)
        target_link_options(RICKY_CPP PRIVATE ${LINKER_OPTIONS})
    endif ()
elseif (MSVC)
    target_compile_options(RICKY_CPP PRIVATE /bigobj)
endif ()

# -------------------------------------------------------------------
# 输出目录与子项目
# -------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加测试和示例目录
add_subdirectory(tests)
enable_testing()

if (EXISTS ${CMAKE_SOURCE_DIR}/example)
    add_subdirectory(example)
endif ()