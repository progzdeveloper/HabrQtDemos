cmake_minimum_required(VERSION 3.5)

project(ShapedWidget LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(ShapedWidget
    main.cpp
    screenframe.cpp
    screenframe.h
    shapedwidget.cpp
    shapedwidget.h
  )

target_link_libraries(ShapedWidget PRIVATE Qt5::Widgets)
