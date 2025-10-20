TEMPLATE = subdirs
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS += encode \
           decode \
           bank
