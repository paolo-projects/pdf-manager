# PDF Manager

An open source cross-platform Qt application to extract pages from a PDF file and save them to another one.

### Compile
To compile the software you need Qt (qmake or qtcreator).

You also need to compile or get the [muPdf](https://www.mupdf.com/downloads/index.html) library, needed to open/create/render PDF files. On Linux you can probably find a compiled version in your package manager while on windows you have to compile it yourself, and then edit the .pro file accordingly to point to your lib and headers locations.
```...
win32: {
INCLUDEPATH += C:\mupdf-1.16.0-source\include
}

win32: {
LIBS += -LC:\mupdf-1.16.0-source\platform\win32\Release
}
...```
