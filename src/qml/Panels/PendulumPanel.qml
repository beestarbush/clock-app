import QtQuick

import Components
import Bee as Backend

RoundPanel {
	id: pendulumPanel

	anchors.fill: parent

    backgroundColor: Backend.Applications.setup.pendulumBackgroundColor

    Pendulum {
        id: pendulum

        bobColor: Backend.Applications.setup.pendulumBobColor
        rodColor: Backend.Applications.setup.pendulumRodColor
        anchors.fill: parent
        active: Backend.Applications.setup.setupComplete || (!Backend.Applications.setup.setupComplete && Backend.Applications.setup.currentPanel === Backend.SetupEnums.Finish)
    }

    // Find the first clock app from the full application list.
    // Re-evaluated whenever apps are reloaded.
    property var clockApp: {
        var _ = Backend.Applications.reloading // depend on reloading so this re-evaluates after reload
        var ids = Backend.Applications.applicationIds
        for (var i = 0; i < ids.length; i++) {
            var app = Backend.Applications.application(ids[i])
            if (app && app.type === Backend.Common.Type.Clock) return app
        }
        return null
    }

    Connections {
        target: pendulumPanel.clockApp
        function onTicked() { pendulum.tick() }
    }
}