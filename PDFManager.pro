#-------------------------------------------------
#
# Project created by QtCreator 2019-09-03T16:35:12
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PDFManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 static

SOURCES += \
        aboutdialog.cpp \
        customdraglistview.cpp \
        listnavigationeventfilter.cpp \
        main.cpp \
        mainwindow.cpp \
        pdfmultiplepagesspecificator.cpp \
        pdfnewdocument.cpp \
        pdfpagecontinuousintervalspecificator.cpp \
        pdfpixmappainter.cpp \
        pdfrangesitemmodel.cpp \
        pdfrenderedpage.cpp \
        pdfsinglepagespecificator.cpp \
        pdfutil.cpp

HEADERS += \
        aboutdialog.h \
        customdraglistview.h \
        listnavigationeventfilter.h \
        mainwindow.h \
        pdfexception.h \
        pdfmultiplepagesspecificator.h \
        pdfnewdocument.h \
        pdfpagecontinuousintervalspecificator.h \
        pdfpagerangespecificator.h \
        pdfpixmappainter.h \
        pdfrangesitemmodel.h \
        pdfrenderedpage.h \
        pdfsinglepagespecificator.h \
        pdfutil.h \
        progsettings.h

FORMS += \
        aboutdialog.ui \
        mainwindow.ui

win32: {
INCLUDEPATH += C:\mupdf-1.16.0-source\include
}

win32: {
LIBS += -LC:\mupdf-1.16.0-source\platform\win32\Release
}

LIBS += -llibmupdf

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md

RESOURCES += \
    resources.qrc

win32: {
RC_ICONS = appicon.ico
}
