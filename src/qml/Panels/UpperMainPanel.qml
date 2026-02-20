import QtQuick

import Components
import Bee as Backend

Circle {
    id: upperMainPanel

    color: Color.blue // This should not be visible, if it is then some formatting is wrong.
    property Item lowerMenuOverlay

    PanelContainer {
        id: panelContainer
        anchors.fill: parent

        property bool isLoading: Backend.Applications.reloading || Backend.Services.configuration.startupCheckInProgress || Backend.Services.media.startupCheckInProgress
        property Panel initialPanel: setupPanel.enabled ? setupPanel : watchfacesPanel

        currentIndex: {
            if (isLoading) return indexOfPanel(loadingPanel)
            if (setupPanel.enabled) return indexOfPanel(setupPanel)
            return indexOfPanel(watchfacesPanel)
        }

        SetupPanel {
            id: setupPanel

            anchors.fill: parent
            enabled: !Backend.Applications.setup.setupComplete
            lowerMenuOverlay: upperMainPanel.lowerMenuOverlay

            onFinished: {
                panelContainer.showPanel(watchfacesPanel)
            }

            onEnabledChanged: {
                if (enabled) {
                    panelContainer.showPanel(setupPanel)
                    menuOverlay.visible = false
                }
            }
        }

        WatchfacesPanel {
            id: watchfacesPanel
            anchors.fill: parent
            onClicked: menuOverlay.visible = true
        }

        LoadingPanel {
            id: loadingPanel
            anchors.fill: parent
        }
    }

    UpperMenuOverlay {
        id: menuOverlay

        visible: false
        anchors.fill: parent

        onClose: {
            menuOverlay.visible = false
            Backend.Applications.menu.closeDialog()
        }
    }

    NotificationOverlay {
        id: notificationOverlay
        anchors.fill: parent

        z: 1000 // Ensure it appears above other content
    }
}
