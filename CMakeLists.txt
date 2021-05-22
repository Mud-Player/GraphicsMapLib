cmake_minimum_required(VERSION 3.5)

project(GraphicsMapLib LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(${PROJECT_NAME} SHARED
  GraphicsMapLib_global.h
  graphicsmap.h
  interactivemap.h
  mapellipseitem.h
  mapellipseoperator.h
  mappolygonitem.h
  maptrailitem.h
  mapobjectitem.h
  graphicsmap.cpp
  interactivemap.cpp
  mapellipseitem.cpp
  mapellipseoperator.cpp
  mappolygonitem.cpp
  maptrailitem.cpp
  mapobjectitem.cpp
)
add_library(Lib::GraphicsMap ALIAS ${PROJECT_NAME})

#
find_package(Qt5 COMPONENTS Core Widgets Positioning REQUIRED)

#
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Widgets Qt5::Positioning)

#
target_compile_definitions(${PROJECT_NAME} PRIVATE GRAPHICSMAPLIB_LIBRARY)

#
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION install)
