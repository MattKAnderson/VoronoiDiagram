add_library(test_common STATIC common.cpp)
target_include_directories(test_common PUBLIC ${PROJECT_SOURCE_DIR})