# ===================================================================
# examples配置
# ===================================================================
cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------------------------
# 目标配置
# -------------------------------------------------------------------
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS *.cpp *.cxx)

foreach (example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    add_executable(${example_name} ${example_source})

    # 继承主库配置
    target_link_libraries(${example_name} PRIVATE ricky_cpp::ricky_cpp)

    # 输出目录
    set_target_properties(${example_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
    )

    # 控制台应用程序配置
    if (WIN32)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_link_options(${example_name} PRIVATE -Wl,-subsystem,console)
        elseif (MSVC)
            set_target_properties(${example_name} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
        endif ()
    endif ()
endforeach ()
