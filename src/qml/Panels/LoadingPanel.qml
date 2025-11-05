import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: loadingPanel

    backgroundColor: Color.black

    Item {
        anchors.fill: parent

        Column {
            anchors.centerIn: parent
            spacing: parent.width * 0.03

            Text {
                id: loadingText

                anchors.horizontalCenter: parent.horizontalCenter
                color: Color.lightGray
                font.pointSize: loadingPanel.width > 0 ? loadingPanel.width * 0.04 : Value.defaultTextSize
                text: Translation.loadingPanelLabelText
            }
        }
    }
}
