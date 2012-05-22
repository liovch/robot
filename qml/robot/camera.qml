import QtQuick 1.1

Rectangle {
    id: root
    width: 854
    height: 480
    color: "green"

    signal qmlClicked()

    MouseArea {
        anchors.fill: parent
        onPressed: root.color = "red"
        onReleased: root.color = "green"
        onClicked: {
            root.qmlClicked()
        }
    }
} 
