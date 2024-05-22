set(SRCS VoronoiDiagram.cpp Impl/BeachLine.cpp Impl/Event.cpp 
         Impl/geometry.cpp Impl/LineClipper.cpp)
add_library(VoronoiDiagram STATIC ${SRCS})
add_library(VoronoiDiagram::VoronoiDiagram ALIAS VoronoiDiagram)
target_include_directories(VoronoiDiagram PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_features(VoronoiDiagram PUBLIC cxx_std_11)