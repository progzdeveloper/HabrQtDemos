cmake_minimum_required(VERSION 3.5)

project(CustomButton LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(CustomButton
    resources.qrc
    main.cpp
    custombutton.cpp
    custombutton.h
    widget.cpp
    widget.h
  )

target_link_libraries(CustomButton PRIVATE Qt5::Widgets)
