add_executable(dual_graph_example dual_graph_example.cpp)
target_link_libraries(dual_graph_example VoronoiDiagram example_file_output)
target_include_directories(dual_graph_example PUBLIC ${PROJECT_SOURCE_DIR})