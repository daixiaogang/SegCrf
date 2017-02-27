TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    gco-v3.0/GCoptimization.cpp \
    gco-v3.0/graph.cpp \
    gco-v3.0/LinkedBlockList.cpp \
    gco-v3.0/maxflow.cpp \
    crf.cpp \
    data.cpp

DISTFILES += \
    gco-v3.0/GCO_README.TXT

HEADERS += \
    gco-v3.0/block.h \
    gco-v3.0/energy.h \
    gco-v3.0/GCoptimization.h \
    gco-v3.0/graph.h \
    gco-v3.0/LinkedBlockList.h \
    crf.h \
    data.h \
    global.h

INCLUDEPATH += /usr/local/include

LIBS += -L /usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
