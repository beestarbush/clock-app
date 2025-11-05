import QtQuick

import Components
import Bee as Backend

RoundPanel {
	id: setupPanel

    backgroundColor: Color.black

    property Item lowerMenuOverlay
    property var setup: Backend.Applications.setup

    signal finished()

    // Listen to dial wheel value changes
    Connections {
        target: lowerMenuOverlay

        function onDialWheelValueUpdated(newValue) {
            setup.updateDialWheelValue(newValue)

            if (setup.currentPanel === Backend.SetupEnums.AppDateTime) {
                appDateTimePanel.updateSelectedComponent(newValue)
            } else if (setup.currentPanel === Backend.SetupEnums.DeviceId) {
                deviceIdPanel.valueText = `SN-${newValue.toString().padStart(4, '0')}`
            } else if (setup.currentPanel === Backend.SetupEnums.AppOpacity) {
                if (setup.currentApp && setup.currentApp.configuration) {
                    setup.currentApp.configuration.backgroundOpacity = newValue / 100.0
                }
            }
        }
    }

    // React to backend panel changes
    Connections {
        target: setup

        function onCurrentPanelChanged() {
            switch(setup.currentPanel) {
                case Backend.SetupEnums.Welcome:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(welcomePanel)
                    break
                case Backend.SetupEnums.DeviceId:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(deviceIdPanel)
                    break
                case Backend.SetupEnums.ServerConnection:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(serverConnectionPanel)
                    break
                case Backend.SetupEnums.AppEnable:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appEnablePanel)
                    break
                case Backend.SetupEnums.AppDateTime:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appDateTimePanel)
                    appDateTimePanel.selectedComponent = -1
                    appDateTimePanel.dateTime = setup.currentApp && setup.currentApp.configuration && setup.currentApp.configuration.timestamp
                        ? new Date(setup.currentApp.configuration.timestamp * 1000)
                        : new Date()
                    break
                case Backend.SetupEnums.AppBackground:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appBackgroundPanel)
                    break
                case Backend.SetupEnums.AppOpacity:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appOpacityPanel)
                    break
                case Backend.SetupEnums.AppBaseColor:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appBaseColorPanel)
                    break
                case Backend.SetupEnums.AppAccentColor:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(appAccentColorPanel)
                    break
                case Backend.SetupEnums.Finish:
                    panelContainer.currentIndex = panelContainer.indexOfPanel(finishPanel)
                    break
            }
        }
    }

    PanelContainer {
        id: panelContainer
        anchors.fill: parent

        property Panel initialPanel: welcomePanel

        currentIndex: {
            if (!setup) return indexOfPanel(welcomePanel)
            switch(setup.currentPanel) {
                case Backend.SetupEnums.Welcome: return indexOfPanel(welcomePanel)
                case Backend.SetupEnums.DeviceId: return indexOfPanel(deviceIdPanel)
                case Backend.SetupEnums.ServerConnection: return indexOfPanel(serverConnectionPanel)
                case Backend.SetupEnums.AppEnable: return indexOfPanel(appEnablePanel)
                case Backend.SetupEnums.AppDateTime: return indexOfPanel(appDateTimePanel)
                case Backend.SetupEnums.AppBackground: return indexOfPanel(appBackgroundPanel)
                case Backend.SetupEnums.AppOpacity: return indexOfPanel(appOpacityPanel)
                case Backend.SetupEnums.AppBaseColor: return indexOfPanel(appBaseColorPanel)
                case Backend.SetupEnums.AppAccentColor: return indexOfPanel(appAccentColorPanel)
                case Backend.SetupEnums.Finish: return indexOfPanel(finishPanel)
                default: return indexOfPanel(welcomePanel)
            }
        }

        TitlePanel {
            id: welcomePanel

            anchors.fill: parent
            titleText: Translation.setupPanelWelcomeTitleText
            descriptionText: Translation.setupPanelWelcomeDescriptionText
            buttonText: Translation.setupPanelWelcomeButtonText

            onButtonClicked: setup.next()
        }

        IntegerSelectPanel {
            id: deviceIdPanel

            anchors.fill: parent
            titleText: Translation.setupPanelRegisterTitleText
            descriptionText: Translation.setupPanelRegisterDescriptionText
            valueText: Backend.Services.remoteApi.deviceId
            valueTextSelected: setup && setup.dialWheel ? setup.dialWheel.visible : false

            onValueSelected: setup.showDialWheel(0, 10, 1, 0)

            onButtonClicked: {
                Backend.Services.remoteApi.deviceId = deviceIdPanel.valueText
                setup.next()
            }
        }

        ToggleButtonPanel {
            id: serverConnectionPanel

            anchors.fill: parent
            titleText: Translation.setupPanelServerConnectionTitleText
            descriptionText: Translation.setupPanelServerConnectionDescriptionText
            toggleTarget: Backend.Services.remoteApi
            toggleProperty: "enabled"

            onButtonClicked: setup.next()
        }

        ToggleButtonPanel {
            id: appEnablePanel

            anchors.fill: parent
            titleText: setup.currentApp ? setup.currentApp.displayName : ""
            descriptionText: Translation.setupPanelAppEnableDescriptionText
            toggleTarget: setup.currentApp ? setup.currentApp.configuration : null
            toggleProperty: "enabled"

            onButtonClicked: setup.next()
        }

        DateTimePickerPanel {
            id: appDateTimePanel

            anchors.fill: parent
            titleText: setup.currentApp ? Translation.setupPanelAppDateTimePickerTitleText.arg(setup.currentApp.displayName) : ""
            descriptionText: Translation.setupPanelAppDateTimePickerDescriptionText

            onComponentSelected: function(component) {
                var d = dateTime
                setup.showDateTimeComponentPicker(component, d.getFullYear(), d.getMonth() + 1,
                    d.getDate(), d.getHours(), d.getMinutes(), d.getSeconds())
            }

            onButtonClicked: {
                var selectedTimestamp = Math.floor(dateTime.getTime() / 1000)
                setup.currentApp.configuration.timestamp = selectedTimestamp
                setup.currentApp.configuration.initialized = true
                setup.next()
            }
        }

        TitlePanel {
            id: appBackgroundPanel

            anchors.fill: parent
            titleText: setup.currentApp ? Translation.setupPanelAppBackgroundTitleText.arg(setup.currentApp.displayName) : ""
            descriptionText: Translation.setupPanelAppBackgroundDescriptionText
            buttonText: Translation.setupPanelAppBackgroundButtonText

            onButtonClicked: setup.next()
        }

        TitlePanel {
            id: appOpacityPanel

            anchors.fill: parent
            titleText: setup.currentApp ? Translation.setupPanelAppOpacityTitleText.arg(setup.currentApp.displayName) : ""
            descriptionText: Translation.setupPanelAppOpacityDescriptionText
            buttonText: Translation.setupPanelAppOpacityButtonText

            onButtonClicked: setup.next()
        }

        TitlePanel {
            id: appBaseColorPanel

            anchors.fill: parent
            titleText: setup.currentApp ? Translation.setupPanelAppBaseColorTitleText.arg(setup.currentApp.displayName) : ""
            descriptionText: Translation.setupPanelAppBaseColorDescriptionText
            buttonText: Translation.setupPanelAppBaseColorButtonText

            onButtonClicked: setup.next()
        }

        TitlePanel {
            id: appAccentColorPanel

            anchors.fill: parent
            titleText: setup.currentApp ? Translation.setupPanelAppAccentColorTitleText.arg(setup.currentApp.displayName) : ""
            descriptionText: Translation.setupPanelAppAccentColorDescriptionText
            buttonText: Translation.setupPanelAppAccentColorButtonText

            onButtonClicked: setup.next()
        }

        TitlePanel {
            id: finishPanel

            anchors.fill: parent
            titleText: Translation.setupPanelFinishTitleText
            descriptionText: Translation.setupPanelFinishDescriptionText
            buttonText: Translation.setupPanelFinishButtonText

            onButtonClicked: {
                setup.finish()
                setupPanel.finished()
            }
        }
    }
}
