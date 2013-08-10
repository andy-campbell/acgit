######################################################################
# Automatically generated by qmake (3.0) Sat Jan 26 10:29:14 2013
######################################################################

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = acgit

QMAKE_CXXFLAGS += -std=c++11 -ggdb3

INCLUDEPATH += $$PWD/../libAcGit/
DEPENDPATH += $$PWD/../libAcGit/

LIBS += -llibAcGit -L../libAcGit/

QMAKE_RPATHDIR += $$PWD/../libAcGit/
# Input
HEADERS += mainwindow.h  \
           commit.h \
    currentdiff.h

FORMS += mainwindow.ui
SOURCES += mainwindow.cpp \
           main.cpp \
           commit.cpp \
    currentdiff.cpp

HEADERS += \
    revviewdelegate.h

SOURCES += \
    revviewdelegate.cpp

HEADERS +=

SOURCES +=

HEADERS += \
    difftextedit.h

SOURCES += \
    difftextedit.cpp

OTHER_FILES += \
    README \
    TODO.txt

RESOURCES += \
    acGitResource.qrc

HEADERS += \
    mainwindowrevview.h

SOURCES += \
    mainwindowrevview.cpp

HEADERS += \
    mainwindowrevview.h

HEADERS += \
    fileschangesview.h

SOURCES += \
    fileschangesview.cpp

