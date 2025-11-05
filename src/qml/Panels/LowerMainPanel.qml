import QtQuick

import Components
import Bee as Backend

Circle {
    id: lowerMainPanel

    color: Color.blue // This should not be visible, if it is then some formatting is wrong.
    property alias menuOverlay: menuOverlay

    PanelContainer {
        id: panelContainer
        anchors.fill: parent

        property Panel initialPanel: pendulumPanel

        currentIndex: indexOfPanel(initialPanel)

        PendulumPanel {
            id: pendulumPanel
        }
    }

    LowerMenuOverlay {
        id: menuOverlay

        // This menu shall be visible when: any menu dialog is active, or setup is not complete and a setup dialog is visible.
        visible: Backend.Applications.menu.dialog !== Backend.MenuEnums.None || 
                 (!Backend.Applications.setup.setupComplete && 
                  (Backend.Applications.setup.dialWheel.visible || Backend.Applications.setup.mediaSelection.visible || Backend.Applications.setup.colorSelection.visible))
        anchors.fill: parent

        onClose: Backend.Applications.menu.closeDialog()
    }
}