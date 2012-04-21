import QtQuick 1.0

Item {
    id: slider; width: 400; height: 16

    // value is read/write.
    property real value: 1
    onValueChanged: updatePos();
    property real maximum: 1
    property real minimum: 1
    property int xMax: width - handle.width - 4
    onXMaxChanged: updatePos();
    onMinimumChanged: updatePos();

    function updatePos() {
        if (maximum > minimum) {
            var pos = 2 + (value - minimum) * slider.xMax / (maximum - minimum);
            pos = Math.min(pos, width - handle.width - 2);
            pos = Math.max(pos, 2);
            handle.x = pos;
        } else {
            handle.x = 2;
        }
    }

    //groove
    Rectangle {
        anchors.fill: parent
        border.color: "#9c9c9c"; radius: 8
        smooth: true
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#bbbbbb" }
            GradientStop { position: 1.0; color: "#dddddd" }
        }
    }

    //handle
    Rectangle {
        id: handle; smooth: true
        y: 2; width: 30; height: slider.height-4; radius: 6
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightsteelblue" }
            GradientStop { position: 1.0; color: "steelblue" }
        }
        border.color: "#9c9c9c"

        MouseArea {
            id: mouseHandle
            anchors.fill: parent; drag.target: parent
            drag.axis: Drag.XAxis; drag.minimumX: 2; drag.maximumX: slider.xMax+2
            onPositionChanged: { value = (maximum - minimum) * (handle.x-2) / slider.xMax + minimum; }
        }
    }
}
