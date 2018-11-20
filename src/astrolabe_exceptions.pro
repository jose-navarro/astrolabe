#-------------------------------------------------
#
# Project created by QtCreator 2018-10-17T08:23:17
#
#-------------------------------------------------

QT       -= core gui

TARGET = astrolabe_exceptions
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    astrolabe_exception.cpp \
    astrolabe_time.cpp \
    astrolabe_contract_violation.cpp \
    astrolabe_precondition_violation.cpp \
    astrolabe_postcondition_violation.cpp

HEADERS += \
        astrolabe_exceptions.hpp \
    astrolabe_exception.hpp \
    astrolabe_time.hpp \
    astrolabe_contracts.hpp \
    astrolabe_contract_violation.hpp \
    astrolabe_precondition_violation.hpp \
    astrolabe_postcondition_violation.hpp
unix {
    target.path = /usr/lib
    INSTALLS += target
}
