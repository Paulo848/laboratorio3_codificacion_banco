TEMPLATE = app
CONFIG += console c++17
QT -= gui

INCLUDEPATH += ../src

SOURCES += main.cpp \
           ../src/encodec.cpp \
           ../src/utilitis.cpp \
           ../src/decode.cpp \
           ../src/fileManage.cpp \
           ../src/userManage.cpp

HEADERS += ../src/encodec.h \
           ../src/utilitis.h \
           ../src/decode.h \
           ../src/fileManage.h \
           ../src/userManage.h
