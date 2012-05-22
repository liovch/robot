# Add more folders to ship with the application, here
folder_01.source = qml/robot
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE3110CC5

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

contains(MEEGO_EDITION,harmattan) {
    QT += declarative
    CONFIG += mobility
    MOBILITY = multimedia
}


# ARToolkitPlus
DEFINES += USE_AR_TOOLKIT
LIBS += -L$$(ARTKP)/lib -lARToolKitPlus
DEPENDPATH      += $$(ARTKP)/include
INCLUDEPATH     += $$(ARTKP)/include
LIBS            += -L$$(ARTKP)/lib
VERSION         = 2.0.2
LANGUAGE = C++

# Camera source
#DEFINES += USE_DECLARATIVE_CAMERA

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    marker.cpp \
    markerparams.cpp \
    camera.cpp \
    robot.cpp \
    random.cpp \
    particlefilter.cpp \
    movementprovider.cpp \
    movement.cpp \
    particledisplay.cpp \
    manager.cpp \
    imageprovider.cpp \
    markerprocessor.cpp \
    thresholdimageprocessor.cpp \
    imageprocessor.cpp \
    artoolkitimageprocessor.cpp \
    declarativecameraimageprovider.cpp \
    cameraimageprovider.cpp
contains(MEEGO_EDITION,harmattan) {
    SOURCES +=
}
!contains(MEEGO_EDITION,harmattan) {
    SOURCES += folderimageprovider.cpp
}

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

HEADERS += \
    marker.h \
    markerparams.h \
    camera.h \
    robot.h \
    random.h \
    particlefilter.h \
    movementprovider.h \
    movement.h \
    particledisplay.h \
    settings.h \
    manager.h \
    imageprovider.h \
    markerprocessor.h \
    thresholdimageprocessor.h \
    imageprocessor.h \
    artoolkitimageprocessor.h \
    declarativecameraimageprovider.h \
    cameraimageprovider.h
contains(MEEGO_EDITION,harmattan) {
    HEADERS +=
}
!contains(MEEGO_EDITION,harmattan) {
    HEADERS += folderimageprovider.h
}

contains(MEEGO_EDITION,harmattan) {
    QMAKE_LFLAGS += -Wl,--rpath=/opt/robot/lib
}
