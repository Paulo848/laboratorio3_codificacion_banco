TEMPLATE = subdirs
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS += encode \
           decode \
           bank

SOURCES += src/encodec.cpp \
           src/utilitis.cpp

HEADERS += src/encodec.h \
           src/utilitis.h
