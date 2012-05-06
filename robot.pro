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

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    imageprocessor.cpp \
    marker.cpp \
    markerparams.cpp \
    markerprocessor.cpp \
    camera.cpp \
    robot.cpp \
    random.cpp \
    particlefilter.cpp \
    movementprovider.cpp \
    movement.cpp \
    particledisplay.cpp \
    manager.cpp \
    folderimageprovider.cpp

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
    qtc_packaging/debian_harmattan/changelog \
    qml/robot/particles.qml

HEADERS += \
    imageprocessor.h \
    marker.h \
    markerparams.h \
    markerprocessor.h \
    camera.h \
    robot.h \
    random.h \
    particlefilter.h \
    movementprovider.h \
    movement.h \
    particledisplay.h \
    settings.h \
    manager.h \
    folderimageprovider.h
