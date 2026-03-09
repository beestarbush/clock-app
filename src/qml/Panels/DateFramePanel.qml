import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: dateFramePanel

    property alias backgroundSource: dateFramePanelBackground.source
    property alias backgroundOpacity: dateFramePanelBackground.opacity
    property color textColor: "white"
    property string currentDate: ""
    property int year: 0
    property int month: 0
    property int day: 0
    property int dayOfWeek: 0

    backgroundColor: Color.black

    signal clicked()

    Backend.RoundAnimatedImage {
        id: dateFramePanelBackground

        anchors.fill: parent
    }

    Item {
        id: dateContainer

        anchors.fill: parent

        property bool hasDateParts: dateFramePanel.year > 0 && dateFramePanel.month > 0 && dateFramePanel.day > 0 && dateFramePanel.dayOfWeek > 0
        property var monthNames: ["", Translation.dateDisplayPanelJanuary, Translation.dateDisplayPanelFebruary, Translation.dateDisplayPanelMarch, Translation.dateDisplayPanelApril, Translation.dateDisplayPanelMay, Translation.dateDisplayPanelJune, Translation.dateDisplayPanelJuly, Translation.dateDisplayPanelAugust, Translation.dateDisplayPanelSeptember, Translation.dateDisplayPanelOctober, Translation.dateDisplayPanelNovember, Translation.dateDisplayPanelDecember]
        property var dayNames: ["", Translation.dateDisplayPanelMonday, Translation.dateDisplayPanelTuesday, Translation.dateDisplayPanelWednesday, Translation.dateDisplayPanelThursday, Translation.dateDisplayPanelFriday, Translation.dateDisplayPanelSaturday, Translation.dateDisplayPanelSunday]

        Text {
            id: dayOfWeekText

            anchors.bottom: dayText.top
            anchors.bottomMargin: Value.defaultMargin
            anchors.horizontalCenter: parent.horizontalCenter
            text: dateContainer.hasDateParts ? dateContainer.dayNames[dateFramePanel.dayOfWeek].toUpperCase() : ""
            font.pixelSize: Value.largeTextSize
            font.bold: true
            color: dateFramePanel.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            id: dayText

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: dateContainer.hasDateParts ? dateFramePanel.day : ""
            font.pixelSize: parent.width * 0.38
            font.bold: true
            color: dateFramePanel.textColor
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            id: separator

            anchors.top: dayText.bottom
            anchors.topMargin: Value.defaultMargin * 0.35
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.6
            height: 2
            radius: 1
            color: dateFramePanel.textColor
            opacity: 0.5
            visible: dateContainer.hasDateParts
        }

        Text {
            id: monthYearText

            anchors.top: separator.bottom
            anchors.topMargin: Value.defaultMargin * 0.35
            anchors.horizontalCenter: parent.horizontalCenter
            text: dateContainer.hasDateParts ? (dateContainer.monthNames[dateFramePanel.month] + " " + dateFramePanel.year).toUpperCase() : ""
            font.pixelSize: Value.largeTextSize
            font.bold: true
            color: dateFramePanel.textColor
            horizontalAlignment: Text.AlignHCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: dateFramePanel.clicked()
    }
}
