// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//import Robot 1.0

Rectangle {
    width: 1200
    height: 450

    Item {
        width: 800
        height: 450
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

    ListView {
        anchors.left: imageRect.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        model: markerParamsModel
        delegate: Item {
            anchors.left: parent.left
            anchors.right: parent.right
            height: nameMin.height * 4 + redSliderMin.height * 3 + 10

            Item {
                id: minPanel
                width: 200
                anchors.left: parent.left
                Text {
                    id: nameMin
                    color: model.modelData.name
                    text: model.modelData.name + " (min)"
                }

                Text {
                    id: minRText
                    anchors.top: nameMin.bottom
                    text: "min R: " + model.modelData.minR
                }

                Slider {
                    id: redSliderMin
                    anchors.top: minRText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.minR = value
                    minimum: 0; maximum: 255; value: model.modelData.minR
                }

                Text {
                    id: minGText
                    anchors.top: redSliderMin.bottom
                    text: "min G: " + model.modelData.minG
                }

                Slider {
                    id: greenSliderMin
                    anchors.top: minGText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.minG = value
                    minimum: 0; maximum: 255; value: model.modelData.minG
                }

                Text {
                    id: minBText
                    anchors.top: greenSliderMin.bottom
                    text: "min B: " + model.modelData.minB
                }

                Slider {
                    id: blueMinSlider
                    anchors.top: minBText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.minB = value
                    minimum: 0; maximum: 255; value: model.modelData.minB
                }
            }

            Item {
                id: maxPanel
                anchors.left: minPanel.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                Text {
                    id: nameMax
                    color: model.modelData.name
                    text: model.modelData.name + " (max)"
                }

                Text {
                    id: maxRText
                    anchors.top: nameMax.bottom
                    text: "max R: " + model.modelData.maxR
                }

                Slider {
                    id: redSliderMax
                    anchors.top: maxRText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.maxR = value
                    minimum: 0; maximum: 255; value: model.modelData.maxR
                }

                Text {
                    id: maxGText
                    anchors.top: redSliderMax.bottom
                    text: "max G: " + model.modelData.maxG
                }

                Slider {
                    id: greenSliderMax
                    anchors.top: maxGText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.maxG = value
                    minimum: 0; maximum: 255; value: model.modelData.maxG
                }

                Text {
                    id: maxBText
                    anchors.top: greenSliderMax.bottom
                    text: "max B: " + model.modelData.maxB
                }

                Slider {
                    id: blueMaxSlider
                    anchors.top: maxBText.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    onValueChanged: model.modelData.maxB = value
                    minimum: 0; maximum: 255; value: model.modelData.maxB
                }
            }
        }
    }

}
