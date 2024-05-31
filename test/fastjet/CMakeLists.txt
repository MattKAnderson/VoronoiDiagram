add_library(fastjet STATIC IMPORTED)
set_target_properties(fastjet PROPERTIES
    IMPORTED_LOCATION "C:/cpp_libs/fastjet/lib/libfastjet${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
target_include_directories(fastjet INTERFACE "C:/cpp_libs/fastjet/include")

add_executable(time_fastjet time_fastjet.cpp)
target_link_libraries(time_fastjet PUBLIC test_common fastjet)
target_include_directories(time_fastjet PUBLIC ./ ${PROJECT_SOURCE_DIR})
target_compile_definitions(time_fastjet PUBLIC JC_VORONOI_IMPLEMENTATION)
target_compile_options(time_fastjet PRIVATE -mtune=generic -march=native -static)