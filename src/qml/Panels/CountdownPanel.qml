import QtQuick
import QtQuick.Layouts

import Components
import Bee as Backend

RoundPanel {
	id: countdownPanel

    backgroundColor: Color.black
    property alias backgroundSource: background.source
    property alias backgroundOpacity: background.opacity
    property color textColor: Color.lightGray 

    property alias days: daysText.text
    property alias hours: hoursText.text
    property alias minutes: minutesText.text
    property alias seconds: secondsText.text
    property bool initialized
    property int valueTextSize: width * 0.1
    property int labelTextSize: width * 0.05

    signal clicked()

    Backend.RoundAnimatedImage {
        id: background

        anchors.fill: parent
    }

    // Container for progress bars - shown when initialized
    Item {        
        anchors.fill: parent
        visible: countdownPanel.initialized

        GridLayout {
            columns: 2
            anchors.centerIn: parent

            Text {
                id: daysText
                font.pixelSize: valueTextSize
                font.bold: true
                color: countdownPanel.textColor
                horizontalAlignment: Text.AlignRight
                Layout.fillWidth: true
                Layout.rightMargin: Value.defaultMargin
            }
            Text {
                id: daysLabel
                font.pixelSize: labelTextSize
                color: countdownPanel.textColor
                text: Translation.countdownPanelDays
            }
            Text {
                id: hoursText
                font.pixelSize: valueTextSize
                font.bold: true
                color: countdownPanel.textColor
                horizontalAlignment: Text.AlignRight
                Layout.fillWidth: true
                Layout.rightMargin: Value.defaultMargin
            }
            Text {
                id: hoursLabel
                font.pixelSize: labelTextSize
                color: countdownPanel.textColor
                text: Translation.countdownPanelHours
            }
            Text {
                id: minutesText
                font.pixelSize: valueTextSize
                font.bold: true
                color: countdownPanel.textColor
                horizontalAlignment: Text.AlignRight
                Layout.fillWidth: true
                Layout.rightMargin: Value.defaultMargin
            }
            Text {
                id: minutesLabel
                font.pixelSize: labelTextSize
                color: countdownPanel.textColor
                text: Translation.countdownPanelMinutes
            }
            Text {
                id: secondsText
                font.pixelSize: valueTextSize
                font.bold: true
                color: countdownPanel.textColor
                horizontalAlignment: Text.AlignRight
                Layout.fillWidth: true
                Layout.rightMargin: Value.defaultMargin
            }
            Text {
                id: secondsLabel
                font.pixelSize: labelTextSize
                color: countdownPanel.textColor
                text: Translation.countdownPanelSeconds
            }
        }
    }

    // Container for "not set up" message - shown when not initialized
    Item {        
        anchors.fill: parent
        visible: !countdownPanel.initialized
        
        Text {
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            text: Translation.countdownPanelNotSetup
            font.pixelSize: Value.largeTextSize
            font.bold: true
            color: countdownPanel.textColor
        }
    }

    MouseArea {
		anchors.fill: parent
		onClicked: clockPanel.clicked()
	}
}