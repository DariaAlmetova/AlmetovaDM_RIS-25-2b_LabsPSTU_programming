QT += widgets network concurrent

CONFIG += c++17

SOURCES += \
    capturemanager.cpp \
    dictionarydialog.cpp \
    hotkeymanager.cpp \
    main.cpp \
    mainwindow.cpp \
    ocrengine.cpp \
    popup.cpp \
    regionselector.cpp \
    translator.cpp

HEADERS += \
    capturemanager.h \
    dictionarydialog.h \
    hotkeymanager.h \
    mainwindow.h \
    ocrengine.h \
    popup.h \
    regionselector.h \
    translator.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:LIBS += -luser32

VCPKG_ROOT = $$(VCPKG_ROOT)

isEmpty(VCPKG_ROOT) {
    error("VCPKG_ROOT is not set. Set environment variable VCPKG_ROOT, for example: D:/vcpkg/vcpkg")
}

message("Using VCPKG_ROOT = $$VCPKG_ROOT")

INCLUDEPATH += "$$VCPKG_ROOT/installed/x64-windows/include"
LIBS += -L"$$VCPKG_ROOT/installed/x64-windows/lib"

LIBS += -ltesseract55
LIBS += -lleptonica-1.87.0
LIBS += -llibcrypto
LIBS += -llibcurl