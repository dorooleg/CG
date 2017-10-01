QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mandelbrot
TEMPLATE = app

SOURCES += main.cpp\
           window.cpp \
           mandelbrotwidget.cpp

HEADERS += window.h \
           mandelbrotwidget.h

FORMS += window.ui

LIBS += -L"$$PWD/lib" -lglew32s -lglew32 -lOpengl32 -lglfw3dll

INCLUDEPATH += ./include
