TEMPLATE = app
CONFIG += console c++17
QT -= gui

# Para no usar rutas ../ en los #include
INCLUDEPATH += ../src

SOURCES += main.cpp \
           ../src/encodec.cpp \
           ../src/utilitis.cpp

HEADERS += ../src/encodec.h \
           ../src/utilitis.h
