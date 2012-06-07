# Add more folders to ship with the application, here
folder_01.source = qml/robot
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

contains(MEEGO_EDITION,harmattan) {
    # Speed up launching on MeeGo/Harmattan when using applauncherd daemon
    CONFIG += qdeclarative-boostable

    QT += declarative
    CONFIG += mobility
    MOBILITY = multimedia connectivity
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
    movement.cpp \
    particledisplay.cpp \
    manager.cpp \
    imageprovider.cpp \
    markerprocessor.cpp \
    thresholdimageprocessor.cpp \
    imageprocessor.cpp \
    artoolkitimageprocessor.cpp \
    declarativecameraimageprovider.cpp \
    cameraimageprovider.cpp \
    fcamimageprovider.cpp \
    motionplanner.cpp \
    motionproxy.cpp
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
    cameraimageprovider.h \
    fcamimageprovider.h \
    motionplanner.h \
    motionproxy.h
contains(MEEGO_EDITION,harmattan) {
    HEADERS +=
}
!contains(MEEGO_EDITION,harmattan) {
    HEADERS += folderimageprovider.h
}

contains(MEEGO_EDITION,harmattan) {
    QMAKE_LFLAGS += -Wl,--rpath=/opt/robot/lib
    LIBS += -lFCam -ljpeg

    # Classify the application as a camera
    cameraclassify.files += qtc_packaging/debian_harmattan/robot.syspart.conf
    cameraclassify.path = /usr/share/policy/etc/syspart.conf.d
    INSTALLS += cameraclassify
    DEFINES += USE_FCAM
}
