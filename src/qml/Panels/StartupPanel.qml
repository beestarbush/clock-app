import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: startupPanel

    backgroundColor: Color.black // This should not be visible, if it is then some formatting is wrong.

    Text {
        id: title

        anchors.centerIn: parent
        color: Color.lightGray
        font.pointSize: parent.width > 0 ? parent.width * 0.05 : Value.defaultTextSize
        text: Translation.startupPanelLabelText
    }
}
