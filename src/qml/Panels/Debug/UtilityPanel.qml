import QtQuick
import QtQuick.Layouts

import Components
import Bee as Backend

Item {
    id: utilities

    GridLayout {
            id: gridLayout
            columns: 2
            width: utilities.width

            Button {
                text: Translation.debugPanelUtilityReloadText
                onClicked: Backend.Services.configuration.triggerConfigurationChanged()
            }
    }
}
