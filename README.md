# PDF Manager

An open source cross-platform Qt application to extract pages from a PDF file and save them to another one.

### Releases
You can find **Win32 (discontinued)**, **Win64** installers and **Mac OSX 10.12+** release in the *releases* section.

### Compile
To compile the software you need Qt (qmake or qtcreator).

You also need to compile or get the [muPdf](https://www.mupdf.com/downloads/index.html) library, needed to open/create/render PDF files. On Linux you will probably find a compiled version in your package manager (make sure it's the same version *1.16.0* or higher) while on windows you have to compile it yourself, and then edit the .pro file accordingly to point to your lib and headers locations.
```
...
win32: {
MUPDFPATH = C:\mupdf-1.16.0-source #Edit this path with the path to your build mupdf library

INCLUDEPATH += $$MUPDFPATH\include

!contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build")

        ## Windows x86 (32bit) specific build here
        LIBS += -L$$MUPDFPATH\platform\win32\Release # Edit this path to point to your win32 build directory

    } else {
        message("x86_64 build")

        ## Windows x64 (64bit) specific build here
        LIBS += -L$$MUPDFPATH\platform\win32\x64\Release # Edit this path to point to your win64 build directory
    }
}
...
```

### License
The software is provided under GNU AGPL v3.0. See the LICENSE file for more info.

Icons distibuted with the program are from [icons8](https://icons8.com). Check their website for more info.
