import QtQuick 1.1
import Qt 4.7
import QtMultimediaKit 1.1

Item {
    id: root
    width: 854
    height: 480

    signal qmlClicked

    Camera {
        id: camera
        objectName: "camera"
        //focus : visible // to receive focus and capture key events when visible
        anchors.fill: parent

    //        flashMode: Camera.FlashRedEyeReduction
        whiteBalanceMode: Camera.WhiteBalanceFlash
        exposureCompensation: -1.0
        captureResolution: "3552x2000"

        onImageCaptured : {
            //photoPreview.source = preview  // Show the preview in an Image element
        }
    }

    Rectangle {
        width: 854
        height: 3
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: captureButton
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 100
        height: 100
        color: "green"

        MouseArea {
            anchors.fill: parent
            onPressed: captureButton.color = "red"
            onReleased: captureButton.color = "green"
            onClicked: root.qmlClicked()
        }
    }
}

