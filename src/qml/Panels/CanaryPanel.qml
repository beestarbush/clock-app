import QtQuick

import Components
import Bee as Backend

RoundPanel {
    id: canaryPanel

    property var currentApp: null

    backgroundColor: Color.black

    signal clicked()

    // --- Derived data (null-guarded) ---
    property real co2PartsPerMillion: currentApp ? currentApp.co2PartsPerMillion : 0
    property real temperatureCelsius: currentApp ? currentApp.temperatureCelsius : 0
    property real humidityPercentage: currentApp ? currentApp.humidityPercentage : 0
    property int goodThreshold: (currentApp && currentApp.configuration) ? currentApp.configuration.goodThreshold : 400
    property int dangerThreshold: (currentApp && currentApp.configuration) ? currentApp.configuration.dangerThreshold : 1000

    // --- Continuous scaling factor 0.0 (good) → 1.0 (danger) ---
    property real factor: {
        var range = dangerThreshold - goodThreshold
        if (range <= 0) return 0
        return Math.max(0.0, Math.min(1.0, (co2PartsPerMillion - goodThreshold) / range))
    }

    // --- Bob animation parameters ---
    property real bobDuration: 2000 - (factor * 1500)
    property real bobAmplitude: 2 + (factor * 8)

    // --- Canary item (centred, upper portion) ---
    Item {
        id: canaryItem

        width: 80
        height: 100
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -40 + yOffset

        property real yOffset: 0

        rotation: canaryPanel.factor * 180

        Behavior on rotation {
            SpringAnimation { spring: 1.5; damping: 0.4 }
        }

        // Tail — below body
        Rectangle {
            width: 12
            height: 30
            radius: 6
            color: "#FFC200"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 8
            y: 68
            rotation: 20
        }

        // Body
        Rectangle {
            id: body

            width: 50
            height: 60
            radius: 25
            color: "#FFD700"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 30
        }

        // Wing — on body side
        Rectangle {
            width: 30
            height: 20
            radius: 10
            color: "#FFC200"
            anchors.left: body.left
            anchors.leftMargin: -4
            y: body.y + 18
            rotation: -15
        }

        // Head
        Rectangle {
            id: head

            width: 35
            height: 35
            radius: 17
            color: "#FFD700"
            anchors.horizontalCenter: body.horizontalCenter
            anchors.horizontalCenterOffset: 4
            y: body.y - 22
        }

        // Eye
        Rectangle {
            width: 8
            height: 8
            radius: 4
            color: "#1a1a1a"
            x: head.x + head.width * 0.62
            y: head.y + head.height * 0.28
        }

        // Beak
        Rectangle {
            width: 14
            height: 8
            radius: 2
            color: "#FFA500"
            x: head.x + head.width - 4
            y: head.y + head.height * 0.45
            rotation: 10
        }

        // Bob animation
        SequentialAnimation {
            id: bobAnim

            loops: Animation.Infinite
            running: true

            NumberAnimation {
                target: canaryItem
                property: "yOffset"
                from: -canaryPanel.bobAmplitude
                to: canaryPanel.bobAmplitude
                duration: canaryPanel.bobDuration
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                target: canaryItem
                property: "yOffset"
                from: canaryPanel.bobAmplitude
                to: -canaryPanel.bobAmplitude
                duration: canaryPanel.bobDuration
                easing.type: Easing.InOutSine
            }
        }
    }

    onBobDurationChanged: { bobAnim.restart() }
    onBobAmplitudeChanged: { bobAnim.restart() }

    // --- Text readouts ---
    Column {
        id: textColumn

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 18
        spacing: 6

        // Large CO2 readout
        Text {
            id: co2Text

            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(canaryPanel.co2PartsPerMillion) + " ppm"
            font.pixelSize: 28
            font.bold: true
            color: "white"
            horizontalAlignment: Text.AlignHCenter

            style: Text.Outline
            styleColor: Qt.rgba(0, 0, 0, 0.4)
        }

        // Temperature + humidity side by side
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16

            Text {
                text: canaryPanel.temperatureCelsius.toFixed(1) + "\u00B0C"
                font.pixelSize: 14
                color: "white"
                style: Text.Outline
                styleColor: Qt.rgba(0, 0, 0, 0.4)
            }

            Text {
                text: canaryPanel.humidityPercentage.toFixed(1) + "%"
                font.pixelSize: 14
                color: "white"
                style: Text.Outline
                styleColor: Qt.rgba(0, 0, 0, 0.4)
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: canaryPanel.clicked()
    }
}
