cmake_minimum_required(VERSION 3.5)

project(BlurBehindEffect LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(BlurBehindEffect
    resources.qrc
    main.cpp
    vertex.h
    blur.h
    boxblur.cpp
    stackblur.cpp
    glblurfunctions.cpp
    glblurfunctions.h
    blurbehindeffect.cpp
    blurbehindeffect.h
    widget.cpp
    widget.h
    wigglywidget.h
    wigglywidget.cpp
  )

target_link_libraries(BlurBehindEffect PRIVATE Qt5::Widgets)
