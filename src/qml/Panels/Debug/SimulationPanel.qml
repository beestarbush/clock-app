import QtQuick
import Components
import Bee as Backend

Item {
    Column {
        spacing: 10
        padding: 20

        Button {
            text: Translation.debugPanelSimulationPanelTestInfoText
            onClicked: Backend.Services.notification.showInfo("Information", "This is an info notification")
        }

        Button {
            text: Translation.debugPanelSimulationPanelTestWarningText
            onClicked: Backend.Services.notification.showWarning("Warning", "This is a warning notification")
        }

        Button {
            text: Translation.debugPanelSimulationPanelTestErrorText
            onClicked: Backend.Services.notification.showError("Error", "This is an error notification")
        }

        Button {
            text: Translation.debugPanelSimulationPanelClearAllButtonText
            onClicked: Backend.Services.notification.model.clearAll()
        }

        Button {
            text: Translation.debugPanelSimulationPanelClearInactiveButtonText
            onClicked: Backend.Services.notification.model.clearInactive()
        }

        Button {
            text: Translation.debugPanelSimulationPanelInactivateTopButtonText
            onClicked: {
                let highest = Backend.Services.notification.model.highestPriorityNotification
                if (highest && highest.isActive) {
                    Backend.Services.notification.model.setNotificationActive(highest.id, false)
                }
            }
        }

        Text {
            text: Translation.debugPanelSimulationPanelTotalCountLabelText.arg(Backend.Services.notification.model.count)
            color: "white"
        }

        Text {
            text: Translation.debugPanelSimulationPanelActiveCountLabelText.arg(Backend.Services.notification.model.activeCount)
            color: "white"
        }

        Text {
            text: Translation.debugPanelSimulationPanelHasActiveLabelText.arg(Backend.Services.notification.model.hasNotifications)
            color: "white"
        }

        Text {
            text: Translation.debugPanelSimulationPanelIsVisibleLabelText.arg(Backend.Services.notification.isVisible)
            color: "white"
        }
    }
}