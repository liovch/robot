// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 800
    height: 450
    id: root
    Image {
        id: image
        width: parent.width; height: parent.height// TODO: How to reduce the height by the correct amount?
        // Set from C++: source: "qt-logo.png"
        // clip: true
        smooth: true
        fillMode: Image.PreserveAspectFit
        objectName: "image"
    }

    signal qmlClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.qmlClicked()
        }
    }
}
