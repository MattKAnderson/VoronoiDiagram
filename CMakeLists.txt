cmake_minimum_required(VERSION 3.11.0)
project("VoronoiDiagram" VERSION 0.1.0
                         DESCRIPTION ""
                         HOMEPAGE_URL "https://github.com/MattKAnderson/VoronoiDiagram"
                         LANGUAGES CXX)

add_subdirectory(VoronoiDiagram)
add_subdirectory(example)
add_subdirectory(test)

install(
  TARGETS VoronoiDiagram 
  EXPORT ${PROJECT_NAME}_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)