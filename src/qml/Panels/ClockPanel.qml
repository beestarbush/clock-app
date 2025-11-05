import QtQuick

import Components
import Bee as Backend

RoundPanel {
	id: clockPanel

    property alias backgroundImage: clockBackground.source
    property alias backgroundOpacity: clockBackground.opacity

    property alias hourColor: clock.hourColor
    property alias minuteColor: clock.minuteColor
    property alias secondColor: clock.secondColor

    backgroundColor: Color.black

    signal clicked()

    Backend.RoundAnimatedImage {
        id: clockBackground

        anchors.fill: parent
    }

    Clock {
        id: clock

        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - (Value.defaultMargin * 2)
        height: width
    }

    MouseArea {
		anchors.fill: parent
		onClicked: clockPanel.clicked()
	}
}