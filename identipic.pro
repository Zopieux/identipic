TEMPLATE = app
TARGET = identipic
INCLUDEPATH += src/

QT += widgets gui
CONFIG += c++14

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += Magick++

HEADERS += src/FaceOverlayItem.h \
           src/FaceScene.h \
           src/ImageTransform.h \
           src/MainWindow.h \
           src/SizeListModel.h \
           src/SizeComboBox.h

SOURCES += src/FaceOverlayItem.cpp \
           src/FaceScene.cpp \
           src/ImageTransform.cpp \
           src/main.cpp \
           src/MainWindow.cpp \
           src/SizeListModel.cpp \
           src/SizeComboBox.cpp

FORMS += src/MainWindow.ui

RESOURCES += src/main.qrc

TRANSLATIONS += src/identipic_fr.ts
