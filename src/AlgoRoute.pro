QT += core gui widgets svg network

CONFIG += c++17

TARGET = AlgoRoute
TEMPLATE = app

DESTDIR = ../

# Fix Qt Creator unescaped backslash qmake path in Makefile line 317
win32 {
    QMAKE_QMAKE = C:/Qt/6.11.1/mingw_64/bin/qmake.exe
}

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
