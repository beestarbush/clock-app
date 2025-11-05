import QtQuick 2.15
import QtQuick.Controls 2.15

import Components

Item {
    property alias model: repeater.model
    property int itemCount: repeater.model.count
    property real itemAngle: 360 / itemCount
    property real rotationAngle: 0
    property int selectedIndex: 0

    signal itemSelected(int index)

    function reset() {
        rotationAngle = 0
        selectedIndex = 0
        itemSelected(selectedIndex)
    }

    Circle {
        id: ring
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        color: "transparent"
        border.color: Qt.rgba(0, 0, 0, 0.6)
        border.width: 100
        border.pixelAligned: true
        
        Repeater {
            id: repeater
            model: menuModel
            delegate: Item {
                width: 40
                height: 40

                property int index: model.index
                property string label: model.label
                property string icon: model.icon || ""

                property real angle: index * itemAngle - 90
                property real radius: ring.width / 2 - ring.border.width / 2
                property real absoluteAngle: angle

                x: ring.width / 2 + Math.cos(absoluteAngle * Math.PI / 180) * radius - width / 2
                y: ring.height / 2 + Math.sin(absoluteAngle * Math.PI / 180) * radius - height / 2

                Image {
                    id: iconImage
                    anchors.centerIn: parent
                    width: 36
                    height: 36
                    source: icon
                    visible: icon !== ""
                    sourceSize: Qt.size(36, 36)
                    opacity: index === selectedIndex ? 1.0 : 0.6

                    transform: Rotation {
                        origin.x: iconImage.width / 2
                        origin.y: iconImage.height / 2
                        angle: absoluteAngle + 90
                    }

                    Behavior on opacity {
                        NumberAnimation { duration: 300 }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: label
                    font.pixelSize: 12
                    font.bold: index === selectedIndex
                    color: index === selectedIndex ? Color.green1 : Color.white
                    visible: icon === ""

                    transform: Rotation {
                        origin.x: width / 2
                        origin.y: height / 2
                        angle: absoluteAngle + 90
                    }

                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }
                }
            }
        }

        transform: Rotation {
            origin.x: ring.width / 2
            origin.y: ring.height / 2
            angle: rotationAngle
        }
    }

    
MouseArea {
        anchors.fill: parent
        property real lastAngle: 0
        property bool dragging: false

        onPressed: function(event) {
            const dx = event.x - width / 2;
            const dy = event.y - height / 2;
            const distance = Math.sqrt(dx * dx + dy * dy);

            const outerRadius = ring.width / 2;
            const innerRadius = outerRadius - ring.border.width;

            if (distance >= innerRadius && distance <= outerRadius) {
                lastAngle = Math.atan2(dy, dx);
                dragging = true;
            } else {
                dragging = false;
            }
        }

        onPositionChanged: function(event) {
            if (!dragging) return;

            const dx = event.x - width / 2;
            const dy = event.y - height / 2;
            const currentAngle = Math.atan2(dy, dx);
            const delta = (currentAngle - lastAngle) * 180 / Math.PI;

            rotationAngle += delta;
            lastAngle = currentAngle;
        }

        onReleased: {
            if (!dragging) return;

            // With single rotation system: ring rotates, items stay fixed
            // Find which item is closest to the top position
            // When ring rotates by rotationAngle, item positions shift by -rotationAngle
            
            // Calculate how many steps we've rotated
            let steps = Math.round(rotationAngle / itemAngle);
            
            // The selected index: positive rotation moves ring clockwise,
            // which brings lower-indexed items to the top (direction is inverted)
            selectedIndex = ((-steps % itemCount) + itemCount) % itemCount;
            
            // Snap to align selected item at top
            // We want: item selectedIndex to be at the top position
            // Ring rotation needed: -selectedIndex * itemAngle (negative because direction is inverted)
            rotationAngle = -selectedIndex * itemAngle;
            
            itemSelected(selectedIndex);
            dragging = false;
        }
    }
}
