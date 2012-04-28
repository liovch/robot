// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Robot 1.0

Rectangle {
    width: 500
    height: 500

    ParticleDisplay {
        objectName: "display"
        anchors.fill: parent
    }
}
