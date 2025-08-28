# ===================================================================
# 测试项目配置
# ===================================================================
cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------------------------
# 测试目标配置
# -------------------------------------------------------------------
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
add_executable(RICKY_CPP_TESTS ${TEST_SOURCES})

# 继承主项目配置
target_include_directories(RICKY_CPP_TESTS PRIVATE ${INCLUDE_DIRS})
target_compile_options(RICKY_CPP_TESTS PRIVATE $<TARGET_PROPERTY:RICKY_CPP,COMPILE_OPTIONS>)
target_link_options(RICKY_CPP_TESTS PRIVATE $<TARGET_PROPERTY:RICKY_CPP,LINK_OPTIONS>)

# 链接主项目的库
get_target_property(MAIN_LIBS RICKY_CPP LINK_LIBRARIES)
if (MAIN_LIBS)
    target_link_libraries(RICKY_CPP_TESTS PRIVATE ${MAIN_LIBS})
endif ()

# 测试专用配置
target_compile_definitions(RICKY_CPP_TESTS PRIVATE UNIT_TESTING=1)

# 禁用LTO
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RICKY_CPP_TESTS PRIVATE -fno-lto)
    target_link_options(RICKY_CPP_TESTS PRIVATE -fno-lto)
endif ()

# 确保控制台应用程序
if (WIN32)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_link_options(RICKY_CPP_TESTS PRIVATE -Wl,-subsystem,console)
    elseif (MSVC)
        set_target_properties(RICKY_CPP_TESTS PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    endif ()
endif ()

# 大对象支持
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RICKY_CPP_TESTS PRIVATE -Wa,-mbig-obj)
elseif (MSVC)
    target_compile_options(RICKY_CPP_TESTS PRIVATE /bigobj)
endif ()

# -------------------------------------------------------------------
# 输出与测试注册
# -------------------------------------------------------------------
set_target_properties(RICKY_CPP_TESTS PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
add_test(NAME RICKY_CPP_TESTS COMMAND $<TARGET_FILE:RICKY_CPP_TESTS>)
set_tests_properties(RICKY_CPP_TESTS PROPERTIES TIMEOUT 30)