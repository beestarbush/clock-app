import QtQuick

import Components
import Bee as Backend

Panel {
    id: watchfacesPanel

    property var currentApp: Backend.Applications.watchface.currentApp
    property bool currentAppValid: currentApp !== null && currentApp !== undefined

    signal clicked()

    // Internal PanelContainer to manage watchface panels
    PanelContainer {
        id: watchfaceContainer
        anchors.fill: parent
        visible: true

        currentIndex: {
            if (!currentAppValid || currentApp.watchface === Backend.Common.Watchface.None)
                return 0
            if (currentApp.watchface === Backend.Common.Watchface.AnalogClock)
                return 1
            if (currentApp.watchface === Backend.Common.Watchface.SevenSegment)
                return 2
            if (currentApp.watchface === Backend.Common.Watchface.RoundProgressBar)
                return 3
            if (currentApp.watchface === Backend.Common.Watchface.CountdownTimer)
                return 4
            return 0
        }

        RoundPanel {
            id: noWatchfaces

            backgroundColor: Color.black

            Item {
                id: notSetupContainer

                anchors.fill: parent

                Text {
                    anchors.centerIn: parent
                    text: Translation.watchfacesPanelNotSetup
                    font.pixelSize: Value.largeTextSize
                    font.bold: true
                    color: Color.lightGray
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: watchfacesPanel.clicked()
                }
            }
        }

        // Clock Panel
        ClockPanel {
            id: clockPanel

            onClicked: watchfacesPanel.clicked()

            backgroundImage: (currentAppValid && Backend.Services.media.getMediaPath(currentApp.configuration.background)) || ""
            backgroundOpacity: (currentAppValid && currentApp.configuration.backgroundOpacity) || 0
            hourColor: (currentAppValid && currentApp.configuration.hourColor) || "white"
            minuteColor: (currentAppValid && currentApp.configuration.minuteColor) || "white"
            secondColor: (currentAppValid && currentApp.configuration.secondColor) || "white"
        }

        // Seven Segment Panel
        SevenSegmentPanel {
            id: sevenSegmentPanel

            initialized: (currentAppValid && currentApp.configuration.initialized) || false
            years: (currentApp && currentApp.years) || 0
            days: (currentApp && currentApp.days) || 0
            hours: (currentApp && currentApp.hours) || 0
            minutes: (currentApp && currentApp.minutes) || 0
            seconds: (currentApp && currentApp.seconds) || 0
            onClicked: watchfacesPanel.clicked()

            backgroundImage: (currentAppValid && Backend.Services.media.getMediaPath(currentApp.configuration.background)) || ""
            backgroundOpacity: (currentAppValid && currentApp.configuration.backgroundOpacity) || 0
        }

        // Round Progress Bar Panel
        RoundProgressBarPanel {
            initialized: (currentAppValid && currentApp.configuration.initialized) || false
            years: (currentApp && currentApp.years) || 0
            days: (currentApp && currentApp.days) || 0
            daysInWeek: (currentApp && currentApp.daysInWeek) || 0
            weeks: (currentApp && currentApp.weeks) || 0
            hours: (currentApp && currentApp.hours) || 0
            minutes: (currentApp && currentApp.minutes) || 0
            seconds: (currentApp && currentApp.seconds) || 0
            barColor: (currentAppValid && currentApp.configuration.baseColor) || "white"
            textColor: (currentAppValid && currentApp.configuration.accentColor) || "white"
            backgroundSource: (currentAppValid && Backend.Services.media.getMediaPath(currentApp.configuration.background)) || ""
            backgroundOpacity: (currentAppValid && currentApp.configuration.backgroundOpacity) || 0
            onClicked: watchfacesPanel.clicked()
        }

        // Countdown Panel
        CountdownPanel {
            backgroundSource: (currentAppValid && Backend.Services.media.getMediaPath(currentApp.configuration.background)) || ""
            backgroundOpacity: (currentAppValid && currentApp.configuration.backgroundOpacity) || 0
            initialized: (currentAppValid && currentApp.configuration.initialized) || false
            days: (currentApp && currentApp.days) || 0
            hours: (currentApp && currentApp.hours) || 0
            minutes: (currentApp && currentApp.minutes) || 0
            seconds: (currentApp && currentApp.seconds) || 0
            onClicked: watchfacesPanel.clicked()
        }
    }
}
