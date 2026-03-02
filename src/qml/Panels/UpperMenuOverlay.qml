import QtQuick
import QtQuick.Controls

import Components
import Bee as Backend

PanelContainer {
    id: overlay

    currentIndex: indexOfPanel(mainDialog)

    signal close()

    MenuDialog {
        id: mainDialog
        backgroundOpacity: 0

        RingMenu {
            id: mainRingMenu
            anchors.centerIn: parent
            anchors.fill: parent
            model: Backend.Applications.menu.main

            onItemSelected: (index) => {
                Backend.Applications.menu.main.get(index).trigger()
            }

            Circle {
                anchors.centerIn: parent
                width: mainRingMenu.width - 200
                height: width
                color: "transparent"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        overlay.close()
                        Backend.Applications.menu.closeDialog()
                        mainRingMenu.reset()
                    }
                }
            }
        }
    }
}