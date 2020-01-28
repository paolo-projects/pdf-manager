# PDF Manager

An open source cross-platform Qt application to extract pages from a PDF file and save them to another one.

### Releases
##### Windows
You can find **Win64** installers in the *releases* section (and a 32 bit release of an old version).

##### Mac OSX
A **Mac OSX 10.12+** release is provided in the *releases* section.

#### Linux
On Linux you have to compile the software on your own. Follow the instructions below.

### Compile
To compile the software you need Qt (qmake or qtcreator). A copy of the source code can be found here: [https://github.com/qt/qtbase](https://github.com/qt/qtbase), or you can get an installer for the open-source edition at [https://www.qt.io/download-open-source](https://www.qt.io/download-open-source).

You also need to compile or get the [muPdf](https://www.mupdf.com/downloads/index.html) library, needed to open/create/render PDF files. On Linux and MacOS you can simply get the source tarball from the previous link which includes all the needed dependencies to compile and install (**make sure it's the version *1.16.0***), then extract, get into the directory and run:

    make
    sudo make install

And finally open the project dir and run:

    qmake
    make

On windows you have to compile it yourself, and edit the .pro file accordingly to point to your lib and headers locations.
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
It's easier, when compiling, to use the Qt Creator tool to make sure all the paths are retrieved correctly.
### License
The software is provided under GNU AGPL v3.0. See the LICENSE file for more info.

Icons distibuted with the program are from [icons8](https://icons8.com). Check their website for more info.
