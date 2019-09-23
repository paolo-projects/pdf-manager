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

CONFIG += c++11 static # add static if you built mupdf library statically (default behavior of the mupdf provided MSVC project)

INCLUDEPATH = include

SOURCES += \
        src/aboutdialog.cpp \
        src/customdraglistview.cpp \
        src/customdragtreeview.cpp \
        src/customselectionqlabel.cpp \
        src/drageventfilter.cpp \
        src/htmldelegate.cpp \
        src/listnavigationeventfilter.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/openeddocumentstreemodel.cpp \
        src/pagegridwidget.cpp \
        src/pdfmultiplepagesspecificator.cpp \
        src/pdfnewdocument.cpp \
        src/pdfpagecontinuousintervalspecificator.cpp \
        src/pdfpixmappainter.cpp \
        src/pdfrangesitemmodel.cpp \
        src/pdfrenderedpage.cpp \
        src/pdfsinglepagespecificator.cpp \
        src/pdfutil.cpp \
        src/treeitem.cpp

HEADERS += \
      include/aboutdialog.h \
      include/customdraglistview.h \
      include/customdragtreeview.h \
      include/customselectionqlabel.h \
      include/delayedaction.h \
      include/drageventfilter.h \
      include/htmldelegate.h \
      include/listnavigationeventfilter.h \
      include/mainwindow.h \
      include/openeddocumentstreemodel.h \
      include/overlay.h \
      include/pagegridwidget.h \
      include/pdfexception.h \
      include/pdfmultiplepagesspecificator.h \
      include/pdfnewdocument.h \
      include/pdfpagecontinuousintervalspecificator.h \
      include/pdfpagerangespecificator.h \
      include/pdfpixmappainter.h \
      include/pdfrangesitemmodel.h \
      include/pdfrenderedpage.h \
      include/pdfsinglepagespecificator.h \
      include/pdfutil.h \
      include/progsettings.h \
      include/qtooltipper.h \
      include/rawpointer.h \
      include/treeitem.h \
      include/version.h

FORMS += \
        src/aboutdialog.ui \
        src/mainwindow.ui

win32: {
MUPDFPATH = C:\mupdf-1.16.0-source #Edit this path with the path to your build mupdf library

INCLUDEPATH += $$MUPDFPATH\include

!contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build")

        ## Windows x86 (32bit) specific build here
        LIBS += -L$$ßMUPDFPATH\platform\win32\Release # Edit this path to point to your win32 build directory

    } else {
        message("x86_64 build")

        ## Windows x64 (64bit) specific build here
        LIBS += -L$$MUPDFPATH\platform\win32\x64\Release # Edit this path to point to your win64 build directory
    }

    LIBS += -llibmupdf
} else {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lmupdf -lmupdf-third
}

macx: {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
    ICON = appicon.icns
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    resources.rc

RESOURCES += \
    resources.qrc

win32: {
RC_FILE = resources.rc
}
