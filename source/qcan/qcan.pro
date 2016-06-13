#! [0]
TEMPLATE     = lib
CONFIG      += plugin
QT          += widgets
QT          += network
DEFINES     +=
INCLUDEPATH += .
INCLUDEPATH += ./../include
HEADERS     += ./qcan_client.hpp \
               ./qcan_frame.hpp  \
               ./qcan_interface.hpp
SOURCES     += ./qcan_client.cpp \
               ./qcan_frame.cpp
FORMS       +=
RESOURCES   +=
TARGET       = $$qtLibraryTarget(QCanClient)
win32 {
    CONFIG(debug, release|debug):DESTDIR = ../debug/
    CONFIG(release, release|debug):DESTDIR = ../release/
} else {
    DESTDIR    = ../
}
#! [0]

EXAMPLE_FILES = plugin.json
