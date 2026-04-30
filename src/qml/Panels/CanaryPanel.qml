import QtQuick
import Components
import Bee as Backend

RoundPanel {
    id: canaryPanel

    property var currentApp: null

    property alias backgroundImage: canaryBackground.source
    property alias backgroundOpacity: canaryBackground.opacity
    property color textColor: "white"

    backgroundColor: Color.black

    Backend.RoundAnimatedImage {
        id: canaryBackground

        anchors.fill: parent
    }

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

    // --- Canary container (handles rotation) ---
    // Position the item so its pivot point (290/512, 453/511) lands at the panel center.
    Item {
        id: canaryItem

        width: parent.width / 2
        height: width
        clip: true

        x: parent.width / 2 - width * (290 / 512)
        y: parent.height / 2 - height * (453 / 511)

        // Rotate around the pivot point which is now exactly at the panel center
        transform: Rotation {
            origin.x: canaryItem.width * (290 / 512)
            origin.y: canaryItem.height * (453 / 511)
            angle: canaryPanel.factor * 180

            Behavior on angle {
                SpringAnimation { spring: 1.5; damping: 0.4 }
            }
        }

        // --- The PNG Image ---
        Image {
            id: canaryImage
            anchors.fill: parent
            source: "qrc:/Icons/canary.png"
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
        }
    }

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
            font.pixelSize: Value.largeTextSize
            font.bold: true
            color: canaryPanel.textColor
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
                font.pixelSize: Value.defaultTextSize
                color: canaryPanel.textColor
                style: Text.Outline
                styleColor: Qt.rgba(0, 0, 0, 0.4)
            }

            Text {
                text: canaryPanel.humidityPercentage.toFixed(1) + "%"
                font.pixelSize: Value.defaultTextSize
                color: canaryPanel.textColor
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