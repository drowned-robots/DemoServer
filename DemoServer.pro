TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    watcher.cpp \
    logger.cpp \
    transact_handler.cpp


INSTALLS        = target
target.files    = widgettest
target.path     = /home/pi

HEADERS += \
    watcher.h \
    logger.h \
    transact_handler.h
