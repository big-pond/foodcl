#-------------------------------------------------
#
# Project created by QtCreator 2017-11-28T21:23:33
#
#-------------------------------------------------

QT  += core gui sql charts

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = foodcl
TEMPLATE = app

RC_FILE = foodcl.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    mainwdg.cpp \
    def.cpp \
    prodtree.cpp \
    prodwdg.cpp \
    prodeddlg.cpp \
    groupeddlg.cpp \
    grouptree.cpp \
    groupdlg.cpp \
    selproddlg.cpp \
    startwizard.cpp \
    persondlg.cpp \
    indicators.cpp \
    querywdg.cpp \
    chartwdg.cpp \
    masidxwdg.cpp \
    configdlg.cpp \
    querypwdg.cpp \
    proddlg.cpp \
    prodwdgdlg.cpp \
    datedlg.cpp \
    querypfcwdg.cpp \
    spinboxdelegate.cpp

HEADERS  += mainwindow.h \
    mainwdg.h \
    def.h \
    prodtree.h \
    prodwdg.h \
    prodeddlg.h \
    groupeddlg.h \
    grouptree.h \
    groupdlg.h \
    selproddlg.h \
    startwizard.h \
    persondlg.h \
    indicators.h \
    querywdg.h \
    chartwdg.h \
    masidxwdg.h \
    configdlg.h \
    querypwdg.h \
    proddlg.h \
    prodwdgdlg.h \
    datedlg.h \
    querypfcwdg.h \
    spinboxdelegate.h

FORMS    += mainwindow.ui \
    mainwdg.ui \
    prodwdg.ui \
    prodeddlg.ui \
    groupeddlg.ui \
    groupdlg.ui \
    selproddlg.ui \
    querywdg.ui \
    masidxwdg.ui \
    configdlg.ui \
    querypwdg.ui \
    proddlg.ui \
    prodwdgdlg.ui \
    datedlg.ui \
    querypfcwdg.ui

RESOURCES += \
    foodcl.qrc


TRANSLATIONS += foodcl_ru.ts

CODECFORSRC  += UTF-8

