# ===================================================================
# 测试项目CMake配置（tests/CMakeLists.txt）
# ===================================================================
cmake_minimum_required(VERSION 3.20)

# ----------------------------------------
# 测试源码管理
# ----------------------------------------
file(GLOB_RECURSE TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# 测试可执行文件
add_executable(RICKY_CPP_TESTS ${TEST_SOURCES})

# ----------------------------------------
# include目录配置
# ----------------------------------------
target_include_directories(RICKY_CPP_TESTS PUBLIC
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
        ${CMAKE_SOURCE_DIR}/include/ai
        ${CMAKE_SOURCE_DIR}/include/num
        ${CMAKE_SOURCE_DIR}/include/mem
)

# ----------------------------------------
# 编译器优化
# ----------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(RICKY_CPP_TESTS PRIVATE
            -Wa,-mbig-obj             # 汇编器大对象支持
    )

    # 链接器优化（gold/lld）
    include(CheckLinkerFlag)
    check_linker_flag(CXX "-fuse-ld=gold" LINKER_SUPPORTS_GOLD)
    if (LINKER_SUPPORTS_GOLD)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--gc-sections,--no-keep-memory")
        message(STATUS "[MAIN] Using GNU gold linker")
    else ()
        check_linker_flag(CXX "-fuse-ld=lld" LINKER_SUPPORTS_LLD)
        if (LINKER_SUPPORTS_LLD)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -Wl,--gc-sections,--icf=safe")
            message(STATUS "[MAIN] Using LLVM lld linker")
        else ()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections,--no-keep-memory")
            message(STATUS "[MAIN] Using default linker")
        endif ()
    endif ()

elseif (MSVC)
    # MSVC优化配置
    target_compile_options(RICKY_CPP_TESTS PRIVATE
            /bigobj   # 大对象支持
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")  # 链接时代码生成
endif ()

# ----------------------------------------
# 继承主项目的预编译头配置
# ----------------------------------------
if (DEFINED PCH_HEADER AND EXISTS ${PCH_HEADER})
    target_precompile_headers(RICKY_CPP_TESTS PRIVATE ${PCH_HEADER})
    message(STATUS "[TEST] Sharing PCH from main project")
endif ()

# ----------------------------------------
# 测试输出目录配置
# ----------------------------------------
set_target_properties(RICKY_CPP_TESTS PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

# ----------------------------------------
# 注册CTest测试
# ----------------------------------------
add_test(
        NAME RICKY_CPP_TESTS
        COMMAND $<TARGET_FILE:RICKY_CPP_TESTS>
)
message(STATUS "[TEST] Test executable: $<TARGET_FILE_NAME:RICKY_CPP_TESTS>")