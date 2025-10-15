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

if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
    add_compile_options(-Wno-format)
endif()

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

if (EXISTS ${CMAKE_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif ()

# -------------------------------------------------------------------
# 其他配置
# -------------------------------------------------------------------
option(STRICT_FORMAT_CHECK "Enable strict format string checking" OFF)
