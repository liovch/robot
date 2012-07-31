// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//import Robot 1.0

Rectangle {
    width: 610
    height: 355

    Item {
        width: 600
        height: 338
        id: imageRect
        objectName: "imageRect"
        anchors.left: parent.left
        anchors.top: parent.top
        Image {
            id: image
            width: parent.width; height: parent.height
            // Set from C++: source: "qt-logo.png"
            // clip: true
            smooth: true
            fillMode: Image.PreserveAspectFit
            objectName: "image"

            signal qmlClicked()

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    image.qmlClicked()
                }
            }
        }
    }
}
