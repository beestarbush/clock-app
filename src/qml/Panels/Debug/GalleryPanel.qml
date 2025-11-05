import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Components
import Bee

Item {
    id: galleryTab

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.rightMargin: galleryScrollBar.width + Value.defaultMargin * 2
        contentWidth: gridLayout.width
        contentHeight: gridLayout.height

        GridLayout {
            id: gridLayout
            columns: 2
            width: galleryTab.width

            Button {
                text: Translation.debugPanelGallerySimpleButtonText
            }

            RoundButton {
                text: Translation.debugPanelGalleryRoundButtonText
            }

            ToggleButton {
                id: toggleButton

                target: toggleButton
                property: "enabled"
                enabled: true
                size: ToggleButton.Size.Large
            }

            Text {
                text: Translation.debugPanelGalleryTextComponentText
            }

            ColorWheel {
                width: 200
                height: 200

                startColor: Color.green1
            }

            DialWheel {
                width: 200
                height: 200

                wheelColor: Color.black
            }

            MediaCarousel {
                width: 200
                height: 200

                media: Services.media.model

                onMediaSelected: (mediaName) => {
                    console.log("Selected media: " + mediaName)
                }
            }

            DateTimePicker {
                width: 200
                height: 200
                
                selectedTextColor: Color.green1
                defaultTextColor: Color.black
                
                onComponentSelected: function(component) {
                    console.log("Selected component: " + component)
                }
            }

            Circle {
                color: Color.black
                width: 250
                height: 250

                NotificationCarousel {
                    anchors.fill: parent
                
                    model: Services.notification.model
                }
            }

        Item {
                width: 200
                height: 200

                Clock {
                    anchors.fill: parent

                    hourColor: Color.green1
                    minuteColor: Color.red
                    secondColor: Color.blue
                }
            }

            Pendulum {
                width: 200
                height: 200
                active: true
            }

            SevenSegmentDisplay {
                width: 100
                height: 200

                color: Color.green1

                property int currentDigit: 0

                Timer {
                    interval: 500
                    running: true
                    repeat: true
                    onTriggered: {
                        parent.digit = parent.currentDigit
                        parent.currentDigit = (parent.currentDigit + 1) % 10
                    }
                }
            }

            RoundProgressBar {
                width: 200
                height: 200

                color: Color.green1
                thickness: 20
                showLabel: true

                progress: 75
                divisor: 100
            }
        }
    }

    ScrollBar {
        id: galleryScrollBar

        orientation: Qt.Vertical
        policy: ScrollBar.AsNeeded
        anchors.right: parent.right
        anchors.rightMargin: Value.defaultMargin
        anchors.top: flickable.top
        anchors.bottom: flickable.bottom

        contentItem: Rectangle {
            implicitWidth: 6
            implicitHeight: 100
            radius: width / 2
            color: galleryScrollBar.pressed ? Color.darkGray : Color.black
        }
    }

    // Bind ScrollBar to Flickable
    Binding {
        target: galleryScrollBar
        property: "active"
        value: flickable.moving || galleryScrollBar.pressed
    }
    Binding {
        target: galleryScrollBar
        property: "position"
        value: flickable.visibleArea.yPosition
    }
    Binding {
        target: galleryScrollBar
        property: "size"
        value: flickable.visibleArea.heightRatio
    }
}