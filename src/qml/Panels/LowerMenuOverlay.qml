import QtQuick
import QtQuick.Controls

import Components
import Bee as Backend

PanelContainer {
    id: overlay

    property var menu: Backend.Applications.menu
    property var setup: Backend.Applications.setup

    currentIndex: {
        // Setup dialogs take priority only when setup is not complete
        if (setup && !setup.setupComplete) {
            if (setup.dialWheel && setup.dialWheel.visible) {
                return indexOfPanel(dialWheelDialog)
            }
            if (setup.mediaSelection && setup.mediaSelection.visible) {
                return indexOfPanel(mediaSelectionDialog)
            }
            if (setup.colorSelection && setup.colorSelection.visible) {
                return indexOfPanel(colorSelectionDialog)
            }
        }
        
        // Otherwise show menu dialogs
        switch(menu ? menu.dialog : 0) {
            case Backend.MenuEnums.Version: return indexOfPanel(versionDialog)
            case Backend.MenuEnums.SetupWizard: return indexOfPanel(setupWizardDialog)
            case Backend.MenuEnums.ScreenBrightness: return indexOfPanel(screenBrightnessDialog)
            case Backend.MenuEnums.Notifications: return indexOfPanel(notificationDialog)
            case Backend.MenuEnums.DialWheel: return indexOfPanel(dialWheelDialog)
            default: return indexOfPanel(emptyDialog)
        }
    }

    signal close()
    signal dialWheelValueUpdated(int newValue)

    MenuDialog {
        id: emptyDialog
        anchors.fill: parent
    }

    MenuDialog {
        id: versionDialog
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onPressed: longPressTimer.start()
            onReleased: longPressTimer.stop()
            onCanceled: longPressTimer.stop()
        }

        Timer {
            id: longPressTimer
            interval: 2000
            running: false
            repeat: false
            onTriggered: Backend.Applications.debug.panelEnabled = true
        }

        Text {
            id: versionValue
            width: parent.width - Value.defaultMargin
            font.bold: true
            font.pixelSize: Value.largeTextSize
            anchors.centerIn: parent
            text: Backend.Services.version.tag
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Color.lightGray
        }

        Timer {
            id: toggleTextTimer
            interval: 5000
            running: versionDialog.visible
            repeat: true
            property bool showVersion: true

            onTriggered: {
                if (showVersion) {
                    versionValue.text = Backend.Services.version.tag
                    showVersion = false
                } else {
                    versionValue.text = Backend.Services.remoteApi.deviceId
                    showVersion = true
                }
            }
        }
    }

    MenuDialog {
        id: setupWizardDialog
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onPressed: setupWizardLongPressTimer.start()
            onReleased: setupWizardLongPressTimer.stop()
            onCanceled: setupWizardLongPressTimer.stop()
        }

        Timer {
            id: setupWizardLongPressTimer
            interval: 2000
            running: false
            repeat: false
            onTriggered: {
                Backend.Applications.setup.reset()
                menu.closeDialog()
            }
        }

        Text {
            id: setupWizardTextValue
            width: parent.width - Value.defaultMargin
            font.pixelSize: Value.defaultTextSize
            anchors.centerIn: parent
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: Translation.lowerMenuOverlaySetupWizardButton
            color: Color.lightGray
        }     
    }

    MenuDialog {
        id: screenBrightnessDialog

        anchors.fill: parent

        RoundButton {
            id: brightnessButton

            anchors.fill: parent
            anchors.centerIn: parent

            property real minValue: 0
            property real maxValue: 100
            property real step: 1
            property bool incrementMode: true // true: increment, false: decrement

            // The value you want to control
            property real value: Backend.Drivers.screen.brightness

            // Interpolate between Color.gray and Color.green1
            function lerpColor(a, b, t) {
                return Qt.rgba(
                    a.r + (b.r - a.r) * t,
                    a.g + (b.g - a.g) * t,
                    a.b + (b.b - a.b) * t,
                    a.a + (b.a - a.a) * t
                )
            }
            color: lerpColor(Color.gray, Color.green1, value / 100)

            text: Math.round(value) + "%"

            Timer {
                id: brightnessHoldTimer
                interval: 60
                repeat: true
                running: false
                onTriggered: {
                    if (brightnessButton.incrementMode) {
                        if (brightnessButton.value < brightnessButton.maxValue) {
                            brightnessButton.value = Math.min(brightnessButton.value + brightnessButton.step, brightnessButton.maxValue)
                        }
                    } else {
                        if (brightnessButton.value > brightnessButton.minValue) {
                            brightnessButton.value = Math.max(brightnessButton.value - brightnessButton.step, brightnessButton.minValue)
                        }
                    }

                    Backend.Drivers.screen.brightness = brightnessButton.value
                }
            }

            onPressed: brightnessHoldTimer.start()
            onReleased: {
                brightnessHoldTimer.stop()
                // Toggle increment/decrement mode
                incrementMode = !incrementMode
            }
        }
    }

    MenuDialog {
        id: mediaSelectionDialog
        anchors.fill: parent
        anchors.centerIn: parent

        MediaCarousel {
            id: carousel
            anchors.fill: parent
            anchors.centerIn: parent
            media: Backend.Services.media.model

            function updateSelectedMedia() {
                if (setup.currentApp && setup.currentApp.configuration) {
                    carousel.selectMediaByName(setup.currentApp.configuration.background)
                }
            }

            Component.onCompleted: updateSelectedMedia()

            Connections {
                target: setup
                function onMediaSelectionChanged() {
                    carousel.updateSelectedMedia()
                }
            }

            onMediaSelected: (mediaName) => {
                setup.selectMedia(mediaName)
            }
        }
    }

    MenuDialog {
        id: colorSelectionDialog
        anchors.fill: parent
        anchors.centerIn: parent

        ColorWheel {
            id: colorWheel
            anchors.fill: parent
            anchors.centerIn: parent

            startColor: {
                if (setup && !setup.setupComplete && setup.colorSelection && setup.colorSelection.visible) {
                    return setup.colorSelection.startColor
                }
                return "white"
            }

            onColorSelected: (selectedColor) => {
                if (setup && !setup.setupComplete && setup.colorSelection && setup.colorSelection.visible) {
                    setup.selectColor(selectedColor)
                }
            }
        }
    }

    MenuDialog {
        id: notificationDialog

        anchors.fill: parent
        anchors.centerIn: parent

        NotificationCarousel {
            anchors.fill: parent
            anchors.centerIn: parent
            model: Backend.Services.notification.model
        }
    }

    MenuDialog {
        id: dialWheelDialog
        anchors.fill: parent
        anchors.centerIn: parent

        DialWheel {
            id: dialWheel
            anchors.fill: parent
            anchors.centerIn: parent
            minimumValue: (setup && !setup.setupComplete && setup.dialWheel && setup.dialWheel.visible) ? setup.dialWheel.min : 1
            maximumValue: (setup && !setup.setupComplete && setup.dialWheel && setup.dialWheel.visible) ? setup.dialWheel.max : 31
            stepSize: (setup && !setup.setupComplete && setup.dialWheel && setup.dialWheel.visible) ? setup.dialWheel.step : 1
            value: (setup && !setup.setupComplete && setup.dialWheel && setup.dialWheel.visible) ? setup.dialWheel.value : 1

            onValueChanged: {
                if (setup && !setup.setupComplete && setup.dialWheel && setup.dialWheel.visible) {
                    // In setup mode, just emit signal for SetupPanel to handle
                    dialWheelValueUpdated(dialWheel.value)
                } else {
                    // In menu mode, update menu
                    menu.dialWheelValueChanged(dialWheel.value)
                }
            }
        }
    }
}