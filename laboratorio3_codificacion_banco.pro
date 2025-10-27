TEMPLATE = subdirs
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS += encode \
           decode \
           bank

SOURCES += src/encodec.cpp \
           src/utilitis.cpp \
           src/decode.cpp \
           src/fileManage.cpp \
           src/userManage.cpp

HEADERS += src/encodec.h \
           src/utilitis.h \
           src/decode.h \
           src/fileManage.h \
           src/userManage.h
