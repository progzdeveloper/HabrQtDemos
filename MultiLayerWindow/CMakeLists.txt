cmake_minimum_required(VERSION 3.5)

project(MutiLayerWindow LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(MutiLayerWindow
    resources.qrc
    main.cpp
    brushpreview.cpp
    brushpreview.h
    coloredit.cpp
    coloredit.h
    contentwidget.cpp
    contentwidget.h
    controlpanel.cpp
    controlpanel.h
    popuppanel.cpp
    popuppanel.h
    multilayerwindow.cpp
    multilayerwindow.h
    painttool.cpp
    painttool.h
    ../CustomButton/custombutton.h
    ../CustomButton/custombutton.cpp
    ../ShapedWidget/shapedwidget.h
    ../ShapedWidget/shapedwidget.cpp
  )

target_link_libraries(MutiLayerWindow PRIVATE Qt5::Widgets)
