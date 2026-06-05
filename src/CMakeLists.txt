# ===================================================================
# 子目标定义
# ===================================================================
cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------------------------
# 头文件目录配置
# -------------------------------------------------------------------
file(GLOB_RECURSE HEADER_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/include/*")
foreach (dir ${HEADER_DIRS})
    if (IS_DIRECTORY ${dir})
        list(APPEND INCLUDE_DIRS ${dir})
    endif ()
endforeach ()
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# -------------------------------------------------------------------
# ricky_cpp_core — 静态库，包含所有 .cpp 实现
# -------------------------------------------------------------------
file(GLOB_RECURSE CORE_SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.cxx")
list(FILTER CORE_SRC_FILES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(ricky_cpp_core STATIC ${CORE_SRC_FILES})
add_library(ricky_cpp::core ALIAS ricky_cpp_core)

target_include_directories(ricky_cpp_core
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${INCLUDE_DIRS}
)

# 平台特定库
if (WIN32)
    target_link_libraries(ricky_cpp_core PUBLIC ws2_32)
elseif (UNIX)
    target_link_libraries(ricky_cpp_core PUBLIC pthread)
endif ()

# 编译器与链接器配置
if (MINGW)
    target_compile_options(ricky_cpp_core PRIVATE -Wno-format)
endif ()

target_compile_options(ricky_cpp_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
)

target_compile_options(ricky_cpp_core PRIVATE
    $<$<CONFIG:RELEASE>:
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3 -flto>
    >
    $<$<CONFIG:DEBUG>:
    $<$<CXX_COMPILER_ID:MSVC>:/Zi>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-g3>
    >
)

target_link_options(ricky_cpp_core PRIVATE
    $<$<CONFIG:RELEASE>:
    $<$<CXX_COMPILER_ID:MSVC>:/LTCG>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-flto>
    >
)

# GNU/Clang 特定优化
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ricky_cpp_core PRIVATE
        -ffunction-sections -fdata-sections
        $<$<BOOL:${MINGW}>:-Wa,-mbig-obj>
    )
    target_link_options(ricky_cpp_core PRIVATE
        -Wl,--gc-sections $<$<CONFIG:RELEASE>:-Wl,--strip-all>
    )

    set(LINKER_OPTIONS $<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>$<$<CXX_COMPILER_ID:GNU>:-fuse-ld=gold>)
    include(CheckLinkerFlag)
    check_linker_flag(CXX ${LINKER_OPTIONS} LINKER_SUPPORTS_OPTION)
    if (LINKER_SUPPORTS_OPTION)
        target_link_options(ricky_cpp_core PRIVATE ${LINKER_OPTIONS})
    endif ()
elseif (MSVC)
    target_compile_options(ricky_cpp_core PRIVATE /bigobj)
endif ()

# -------------------------------------------------------------------
# ricky_cpp_math — 纯头文件
# -------------------------------------------------------------------
add_library(ricky_cpp_math INTERFACE)
target_include_directories(ricky_cpp_math INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp_math INTERFACE ricky_cpp_core)

# -------------------------------------------------------------------
# ricky_cpp_nn — 纯头文件
# -------------------------------------------------------------------
add_library(ricky_cpp_nn INTERFACE)
target_include_directories(ricky_cpp_nn INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp_nn INTERFACE ricky_cpp_core ricky_cpp_math)

# -------------------------------------------------------------------
# ricky_cpp_json — 纯头文件
# -------------------------------------------------------------------
add_library(ricky_cpp_json INTERFACE)
target_include_directories(ricky_cpp_json INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp_json INTERFACE ricky_cpp_core)

# -------------------------------------------------------------------
# ricky_cpp_log — 纯头文件
# -------------------------------------------------------------------
add_library(ricky_cpp_log INTERFACE)
target_include_directories(ricky_cpp_log INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp_log INTERFACE ricky_cpp_core)

# -------------------------------------------------------------------
# ricky_cpp_graph — 纯头文件
# -------------------------------------------------------------------
add_library(ricky_cpp_graph INTERFACE)
target_include_directories(ricky_cpp_graph INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp_graph INTERFACE ricky_cpp_core)

# -------------------------------------------------------------------
# 聚合库 — 向后兼容（tests 和 examples 链接此目标无需改动）
# -------------------------------------------------------------------
add_library(ricky_cpp INTERFACE)
target_include_directories(ricky_cpp INTERFACE ${CMAKE_SOURCE_DIR}/include ${INCLUDE_DIRS})
target_link_libraries(ricky_cpp INTERFACE
    ricky_cpp_core
    ricky_cpp_math
    ricky_cpp_nn
    ricky_cpp_json
    ricky_cpp_log
    ricky_cpp_graph
)
add_library(ricky_cpp::ricky_cpp ALIAS ricky_cpp)
