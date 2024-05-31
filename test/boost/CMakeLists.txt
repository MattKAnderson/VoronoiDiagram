set(BOOST_ROOT "C:\\cpp_libs") # edit me
find_package(Boost REQUIRED)

add_executable(time_boost time_boost.cpp)
target_link_libraries(time_boost PUBLIC test_common ${Boost_LIBRARIES})
target_include_directories(time_boost PUBLIC ${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_compile_options(time_boost PRIVATE -mtune=generic -march=native -static)