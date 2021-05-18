cmake_minimum_required(VERSION 3.5)

project(GraphicsMapLib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Positioning REQUIRED)

add_library(GraphicsMapLib SHARED
  GraphicsMapLib_global.h
  graphicsmap.h
  mapellipseitem.h
  mappolygonitem.h
  maptrailitem.h
  graphicsmap.cpp
  mapellipseitem.cpp
  mappolygonitem.cpp
  maptrailitem.cpp
)

target_link_libraries(GraphicsMapLib PRIVATE Qt5::Widgets Qt5::Positioning)

target_compile_definitions(GraphicsMapLib PRIVATE GRAPHICSMAPLIB_LIBRARY)

#
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION install)
