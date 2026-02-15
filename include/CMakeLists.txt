# Header-only grouping for IDEs (optional)
add_library(ricky_headers INTERFACE)
target_include_directories(ricky_headers INTERFACE ${CMAKE_SOURCE_DIR}/include)
