QT += core gui widgets svg network

CONFIG += c++17

TARGET = AlgoRoute
TEMPLATE = app


HEADERS += \
    datastructures.h \
    graph.h \
    dijkstra.h \
    astar.h \
    mapwidget.h \
    mainwindow.h

SOURCES += \
    main.cpp \
    graph.cpp \
    dijkstra.cpp \
    astar.cpp \
    mapwidget.cpp \
    mainwindow.cpp

FORMS += \
    ../forms/mainwindow.ui

RESOURCES += \
    resources.qrc
