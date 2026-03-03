import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: dateDisplayPanel

    property alias backgroundSource: dateDisplayBackground.source
    property alias backgroundOpacity: dateDisplayBackground.opacity
    property color textColor: "white"
    property string currentDate: ""

    backgroundColor: Color.black

    signal clicked()

    Backend.RoundAnimatedImage {
        id: dateDisplayBackground

        anchors.fill: parent
    }

    Text {
        id: dateText

        anchors.centerIn: parent
        text: dateDisplayPanel.currentDate
        font.pixelSize: Value.largeTextSize
        font.bold: true
        color: dateDisplayPanel.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: dateDisplayPanel.clicked()
    }
}
