import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: photoFramePanel

    property alias backgroundSource: photoFrameBackground.source
    property alias backgroundOpacity: photoFrameBackground.opacity

    backgroundColor: Color.black

    signal clicked()

    Backend.RoundAnimatedImage {
        id: photoFrameBackground

        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: photoFramePanel.clicked()
    }
}
