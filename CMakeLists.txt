cmake_minimum_required(VERSION 3.5)

project(GraphicsMapLib LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(${PROJECT_NAME} SHARED
  Resources.qrc
  graphicsmap.cpp
  graphicsmap.h
  interactivemap.cpp
  interactivemap.h
  mapellipseitem.cpp
  mapellipseitem.h
  maplabelitem.h
  maplabelitem.cpp
  maplineitem.cpp
  maplineitem.h
  mapobjectitem.cpp
  mapobjectitem.h
  mapoperator.cpp
  mapoperator.h
  mappieitem.cpp
  mappieitem.h
  mappolygonitem.cpp
  mappolygonitem.h
  maprangeringitem.cpp
  maprangeringitem.h
  maprouteitem.cpp
  maprouteitem.h
  maptrailitem.cpp
  maptrailitem.h
  maprectitem.cpp
  maprectitem.h
  maptableitem.h
  maptableitem.cpp
  mapscutcheonitem.h
  mapscutcheonitem.cpp
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
