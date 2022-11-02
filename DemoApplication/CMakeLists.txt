cmake_minimum_required(VERSION 3.5)

project(DemoApplication LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(DemoApplication
    resources.qrc
    main.cpp
    overlaypanel.cpp
    overlaypanel.h
    controlpanel.cpp
    controlpanel.h
    popuppanel.cpp
    popuppanel.h
    mainwindow.cpp
    mainwindow.h
    ../BlurBehindEffect/blurbehindeffect.cpp
    ../BlurBehindEffect/blurbehindeffect.h
    ../BlurBehindEffect/blur.h
    ../BlurBehindEffect/stackblur.cpp
    ../BlurBehindEffect/boxblur.cpp
    ../BlurBehindEffect/glblurfunctions.cpp
    ../BlurBehindEffect/glblurfunctions.h
    ../BlurBehindEffect/vertex.h
    ../MultiLayerWindow/brushpreview.cpp
    ../MultiLayerWindow/brushpreview.h
    ../MultiLayerWindow/coloredit.cpp
    ../MultiLayerWindow/coloredit.h
    ../MultiLayerWindow/contentwidget.cpp
    ../MultiLayerWindow/contentwidget.h
    ../MultiLayerWindow/painttool.cpp
    ../MultiLayerWindow/painttool.h
    ../CustomButton/custombutton.h
    ../CustomButton/custombutton.cpp
    ../ShapedWidget/shapedwidget.h
    ../ShapedWidget/shapedwidget.cpp
  )


target_link_libraries(DemoApplication PRIVATE Qt5::Widgets)
