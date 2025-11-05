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

            RingMenu {
                id: settingsRingMenu
                visible: mainRingMenu.selectedIndex === Backend.MenuEnums.MainSettings
                anchors.centerIn: parent
                width: parent.width - 200
                height: parent.height - 200
                model: Backend.Applications.menu.settings

                onItemSelected: (index) => {
                    Backend.Applications.menu.settings.get(index).trigger()
                }

                onVisibleChanged: {
                    if (visible && settingsRingMenu.selectedIndex >= 0) {
                        Backend.Applications.menu.settings.get(settingsRingMenu.selectedIndex).trigger()
                    }
                }
            }

            Circle {
                anchors.centerIn: parent
                width: settingsRingMenu.visible
                    ? settingsRingMenu.width - 200 
                    : mainRingMenu.width - 200
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