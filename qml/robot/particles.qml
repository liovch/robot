// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Robot 1.0

Rectangle {
    width: 500
    height: 500
    id: root

    ParticleDisplay {
        objectName: "display"
        anchors.fill: parent
    }

    signal qmlClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.qmlClicked()
        }
    }
}
