TEMPLATE = app
CONFIG += console c++17
QT -= gui

INCLUDEPATH += ../src

SOURCES += main.cpp \
           ../src/encodec.cpp \
           ../src/utilitis.cpp

HEADERS += ../src/encodec.h \
           ../src/utilitis.h
