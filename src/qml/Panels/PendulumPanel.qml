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
}