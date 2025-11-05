import QtQuick
import QtQuick.Controls

import Components

Item {
    id: notificationCarousel

    property var model: null
    property int currentIndex: 0
    property int notificationCount: model ? model.count : 0

    function formatTimestamp(timestamp) {
        if (!timestamp) return ""
        var date = new Date(timestamp)
        return Qt.formatDateTime(date, "hh:mm:ss dd-MM-yyyy")
    }

    // Ensure currentIndex stays within bounds when model changes
    onNotificationCountChanged: {
        if (currentIndex >= notificationCount) {
            currentIndex = Math.max(0, notificationCount - 1)
        }
    }

    // No notifications message
    Text {
        id: noNotificationsText
        anchors.centerIn: parent
        text: Translation.notificationCarouselNoMedia
        color: Color.lightGray
        font.pixelSize: Value.defaultTextSize
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        visible: notificationCount === 0
    }

    // Main flickable for swiping through notifications
    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: notificationRow.width
        contentHeight: height
        flickDeceleration: 1500
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        interactive: notificationCount > 1
        visible: notificationCount > 0

        Row {
            id: notificationRow
            height: parent.height

            Repeater {
                model: notificationCarousel.model

                delegate: Item {
                    width: flickable.width
                    height: flickable.height

                    Column {
                        anchors.centerIn: parent
                        width: parent.width - Value.defaultMargin * 8
                        spacing: Value.defaultMargin

                        Text {
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: model.title || ""
                            color: Color.lightGray
                            font.pixelSize: Value.defaultTextSize
                            font.bold: true
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Text {
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: model.message || ""
                            color: Color.lightGray
                            font.pixelSize: Value.defaultTextSize
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: notificationCarousel.formatTimestamp(model.timestamp)
                            color: Color.lightGray
                            font.pixelSize: Value.defaultTextSize * 0.8
                            opacity: 0.6
                        }
                    }
                }
            }
        }

        onMovementEnded: {
            var targetIndex = Math.max(0, Math.min(Math.round(contentX / width), notificationCount - 1))
            if (targetIndex !== currentIndex) {
                currentIndex = targetIndex
            }
            snapAnimation.to = targetIndex * width
            snapAnimation.start()
        }

        NumberAnimation {
            id: snapAnimation
            target: flickable
            property: "contentX"
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    // Navigation arrows
    Repeater {
        model: ["‹", "›"]

        Rectangle {
            required property int index
            required property string modelData

            anchors {
                left: index === 0 ? parent.left : undefined
                right: index === 1 ? parent.right : undefined
                verticalCenter: parent.verticalCenter
                leftMargin: Value.defaultMargin
                rightMargin: Value.defaultMargin
            }
            width: Value.defaultMargin * 2
            height: width
            radius: width / 2
            color: Color.black
            opacity: index === 0 ? (currentIndex > 0 ? 0.7 : 0.3) : (currentIndex < notificationCount - 1 ? 0.7 : 0.3)
            visible: notificationCount > 1

            Text {
                anchors.centerIn: parent
                text: parent.modelData
                color: Color.lightGray
                font.pixelSize: Value.largeTextSize
                font.bold: true
            }

            MouseArea {
                anchors.fill: parent
                enabled: index === 0 ? currentIndex > 0 : currentIndex < notificationCount - 1
                onClicked: {
                    currentIndex = index === 0 ? Math.max(0, currentIndex - 1) : Math.min(notificationCount - 1, currentIndex + 1)
                    snapAnimation.to = currentIndex * flickable.width
                    snapAnimation.start()
                }
            }
        }
    }
}
