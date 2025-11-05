import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Components
import Bee as Backend

Rectangle {
	id: debugPanel

	color: Color.lightGray

	signal closeButtonClicked()

	// Debugpanel
	// Top centered date and time and timezone
	// Tabs: Gallery, Utility, Inspector, Simulation, Loggin, HMI
	// 	Gallery: show all elements that we use in UI.
	// 	Utility: skip auth, quit, reboot, reset etc.
	// 	Inspector: drop down with all available objects in QML, on click it provides all the properties in this object.
	// 	Simulation: simulate features of application
	// 	Logging: show logs of application on screen
	// 	HMI: Show additional HMI elements, animation speed, mouse events, keyboard events
	// All tabs have toggle button to show panel in separate window

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		RowLayout {
			Layout.fillWidth: true

			Text {
				id: dateTimeDisplay

				Layout.fillWidth: true
				text: Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss t")
				font.pixelSize: 20
				horizontalAlignment: Text.AlignHCenter
				Layout.alignment: Qt.AlignHCenter
			}

			Button {
				text: Translation.debugPanelCloseButton
				onClicked: Backend.Applications.debug.panelEnabled = false
			}
		}

		TabBar {
			id: bar

			Layout.fillWidth: true

			TabButton {
				text: Translation.debugPanelTabHeaderGallery
			}
			TabButton {
				text: Translation.debugPanelTabHeaderUtility
			}
			TabButton {
				text: Translation.debugPanelTabHeaderInspector
			}
			TabButton {
				text: Translation.debugPanelTabHeaderSimulation
			}
			TabButton {
				text: Translation.debugPanelTabHeaderLogging
			}
		}

		StackLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			currentIndex: bar.currentIndex

			GalleryPanel {}
			UtilityPanel {}
			InspectorPanel {}
			SimulationPanel {}
			LoggingPanel {}
		}
	}
}

