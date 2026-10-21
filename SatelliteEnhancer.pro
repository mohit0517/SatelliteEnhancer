QT += core gui widgets

CONFIG += c++17
CONFIG -= debug_and_release

TARGET = SatelliteEnhancer
TEMPLATE = app

INCLUDEPATH += $$PWD/include
INCLUDEPATH += C:/opencv/build/include

CONFIG(debug, debug|release) {
    LIBS += -LC:/opencv/build/x64/vc16/lib -lopencv_world4120d
} else {
    LIBS += -LC:/opencv/build/x64/vc16/lib -lopencv_world4120
}

SOURCES += \
    src/main.cpp \
    src/preprocess.cpp \
    src/linear_enhance.cpp \
    src/nonlinear.cpp \
    src/filters.cpp \
    src/edge_enhance.cpp

HEADERS += \
    include/preprocess.h \
    include/linear_enhance.h \
    include/nonlinear.h \
    include/filters.h \
    include/edge_enhance.h