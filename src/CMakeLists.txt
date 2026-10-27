cmake_minimum_required(VERSION 3.16)

project(AlgoRoute VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../forms)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Network)

add_executable(AlgoRoute
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mapwidget.cpp
    mapwidget.h
    graph.cpp
    graph.h
    dijkstra.cpp
    dijkstra.h
    astar.cpp
    astar.h
    datastructures.h
    resources.qrc
    ../forms/mainwindow.ui
)

target_include_directories(AlgoRoute PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(AlgoRoute PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
)

set_target_properties(AlgoRoute PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
