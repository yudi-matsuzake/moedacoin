#-------------------------------------------------
#
# Project created by QtCreator 2017-03-26T18:21:14
#
#-------------------------------------------------

QT       += core gui
QT       += sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = moedacoin
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS    += -lcrypto

INCLUDEPATH += include

SOURCES += src/main.cpp\
           src/moedacoin.cpp \
           src/mcwallet.cpp \
           src/mcdb.cpp \
    src/publickeyviewdialog.cpp \
    src/transaction.cpp \
    src/moedanetwork.cpp \
    src/request.cpp \
    src/mcsignature.cpp \
    src/mccrypto.cpp

HEADERS  += include/moedacoin.hpp \
    include/mcwallet.hpp \
    include/publickeyviewdialog.hpp \
    include/mcdb.hpp \
    include/transaction.hpp \
    include/moedanetwork.hpp \
    include/request.hpp \
    include/mcsignature.hpp \
    include/mccrypto.hpp

FORMS    += ui/moedacoin.ui \
    ui/publickeyviewdialog.ui

RESOURCES += \
    res/moedacoin.qrc
