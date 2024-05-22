add_library(example_file_output file_output.cpp)
target_include_directories(example_file_output PUBLIC ${PROJECT_SOURCE_DIR})

add_subdirectory(vertex_graph)