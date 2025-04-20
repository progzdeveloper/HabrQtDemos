cmake_minimum_required(VERSION 3.5)

project(ObjectFactory LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_executable(ObjectFactory
    main.cpp
    objectfactory.h
    metaobjectutils.h
    genericfactory.h
    shapefactory.h
    shapefactory.cpp
    shapes.h
    shapes_cxx.h
    shapes_qt.h
)

target_link_libraries(ObjectFactory PRIVATE Qt5::Core)
