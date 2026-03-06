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
            case Backend.MenuEnums.ScreenBrightness: return indexOfPanel(screenBrightnessDialog)
            case Backend.MenuEnums.Volume: return indexOfPanel(volumeDialog)
            case Backend.MenuEnums.Notifications: return indexOfPanel(notificationDialog)
            case Backend.MenuEnums.DialWheel: return indexOfPanel(dialWheelDialog)
            case Backend.MenuEnums.Customize: return indexOfPanel(customizeDialog)
            case Backend.MenuEnums.PowerOff: return indexOfPanel(powerOffDialog)
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
                    versionValue.text = Backend.Applications.setup.deviceId
                    showVersion = true
                }
            }
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
            property real value: Backend.Applications.setup.brightness

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

                    // Send brightness update to backend
                    Backend.Applications.setup.brightness = Math.round(brightnessButton.value);
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
        id: volumeDialog

        anchors.fill: parent

        RoundButton {
            id: volumeButton

            anchors.fill: parent
            anchors.centerIn: parent

            property real minValue: 0
            property real maxValue: 100
            property real step: 1
            property bool incrementMode: true // true: increment, false: decrement

            // The value you want to control
            property real value: Backend.Applications.setup.volume

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
                id: volumeHoldTimer
                interval: 60
                repeat: true
                running: false
                onTriggered: {
                    if (volumeButton.incrementMode) {
                        if (volumeButton.value < volumeButton.maxValue) {
                            volumeButton.value = Math.min(volumeButton.value + volumeButton.step, volumeButton.maxValue)
                        }
                    } else {
                        if (volumeButton.value > volumeButton.minValue) {
                            volumeButton.value = Math.max(volumeButton.value - volumeButton.step, volumeButton.minValue)
                        }
                    }

                    // Send volume update to backend
                    Backend.Applications.setup.volume = Math.round(volumeButton.value);
                }
            }

            onPressed: volumeHoldTimer.start()
            onReleased: {
                volumeHoldTimer.stop()
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

    MenuDialog {
        id: customizeDialog
        anchors.fill: parent
        anchors.centerIn: parent

        property bool networkAvailable: Backend.Drivers.network.ipAddress !== ""

        Text {
            id: settingsLabel
            visible: customizeDialog.networkAvailable
            anchors.bottom: qrCode.top
            anchors.bottomMargin: Value.defaultMargin
            anchors.horizontalCenter: parent.horizontalCenter
            text: Translation.customizeDialogScanQrCodeText
            font.pixelSize: Value.smallTextSize
            font.bold: true
            color: Color.lightGray
            horizontalAlignment: Text.AlignHCenter
        }

        Backend.QrCodeImage {
            id: qrCode
            visible: customizeDialog.networkAvailable
            anchors.centerIn: parent
            anchors.verticalCenterOffset: Value.defaultMargin
            width: Math.min(parent.width, parent.height) * 0.3
            height: width
            text: "http://" + Backend.Drivers.network.ipAddress + ":5173"
            backgroundColor: Color.lightGray
        }

        Text {
            visible: !customizeDialog.networkAvailable
            anchors.centerIn: parent
            width: parent.width - Value.defaultMargin
            text: Translation.customizeDialogNoNetworkText
            font.pixelSize: Value.smallTextSize
            font.bold: true
            color: Color.lightGray
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    MenuDialog {
        id: powerOffDialog
        anchors.fill: parent

        property bool shutdownInitiated: false

        MouseArea {
            anchors.fill: parent
            onPressed: powerOffTimer.start()
            onReleased: {
                powerOffTimer.stop()
                powerOffTimer.progress = 0
            }
            onCanceled: {
                powerOffTimer.stop()
                powerOffTimer.progress = 0
            }
        }

        Timer {
            id: powerOffTimer
            interval: 50
            running: false
            repeat: true

            property int progress: 0
            readonly property int totalSteps: 60 // 3000ms / 50ms = 60 steps

            onTriggered: {
                progress++
                if (progress >= totalSteps) {
                    stop()
                    powerOffDialog.shutdownInitiated = true
                    Backend.Applications.menu.shutdown()
                }
            }
        }

        // Visual feedback circle that fills up
        Circle {
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height) * 0.8
            height: width
            color: "transparent"
            border.width: 4
            border.color: Color.lightGray

            // Progress arc
            Canvas {
                id: progressCanvas
                anchors.fill: parent
                
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    
                    if (powerOffTimer.progress > 0) {
                        var centerX = width / 2
                        var centerY = height / 2
                        var radius = Math.min(width, height) / 2 - 2
                        var startAngle = -Math.PI / 2
                        var progressRatio = powerOffTimer.progress / powerOffTimer.totalSteps
                        var endAngle = startAngle + (2 * Math.PI * progressRatio)
                        
                        ctx.beginPath()
                        ctx.arc(centerX, centerY, radius, startAngle, endAngle, false)
                        ctx.lineWidth = 4
                        ctx.strokeStyle = Color.red
                        ctx.stroke()
                    }
                }
                
                Connections {
                    target: powerOffTimer
                    function onProgressChanged() {
                        progressCanvas.requestPaint()
                    }
                }
            }
        }

        Text {
            anchors.centerIn: parent
            width: parent.width - Value.largeMargin
            font.bold: true
            font.pixelSize: Value.defaultTextSize
            text: powerOffDialog.shutdownInitiated ? Translation.powerOffDialogShuttingDownText : Translation.powerOffDialogHoldText
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: powerOffDialog.shutdownInitiated ? Color.red : Color.lightGray
        }

        onVisibleChanged: {
            if (!visible) {
                shutdownInitiated = false
                powerOffTimer.stop()
                powerOffTimer.progress = 0
            }
        }
    }
}
