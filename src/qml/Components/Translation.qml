pragma Singleton
import QtQuick

Item {
	id: translation

	// Components
	// Media carousel
	readonly property string mediaCarouselNoMedia: qsTr("No media found\nPlace files in media directory.")

	// Notification carousel
	readonly property string notificationCarouselNoMedia: qsTr("No notifications")


	// Panels
	// Countdown panel
	readonly property string countdownPanelDays: qsTr("dagen")
	readonly property string countdownPanelHours: qsTr("uren")
	readonly property string countdownPanelMinutes: qsTr("minuten")
	readonly property string countdownPanelSeconds: qsTr("seconden")
	readonly property string countdownPanelNotSetup: qsTr("Timer not set up yet")

	// Debug panel
	readonly property string debugPanelCloseButton: qsTr("Close")
	readonly property string debugPanelTabHeaderGallery: qsTr("Gallery")
	readonly property string debugPanelTabHeaderUtility: qsTr("Utility")
	readonly property string debugPanelTabHeaderInspector: qsTr("Inspector")
	readonly property string debugPanelTabHeaderSimulation: qsTr("Simulation")
	readonly property string debugPanelTabHeaderLogging: qsTr("Logging")
	readonly property string debugPanelGallerySimpleButtonText: qsTr("Button")
	readonly property string debugPanelGalleryRoundButtonText: qsTr("Round button")
	readonly property string debugPanelGalleryTextComponentText: qsTr("This is a simple text component.")
	readonly property string debugPanelInspectorShowModelText: qsTr("Model")
	readonly property string debugPanelInspectorShowMapText: qsTr("Map")
	readonly property string debugPanelInspectorBackText: qsTr("Back")
	readonly property string debugPanelUtilityShutdownText: qsTr("Shutdown")
	readonly property string debugPanelUtilityRebootText: qsTr("Reboot")
	readonly property string debugPanelUtilityReloadText: qsTr("Configuration reload")
	readonly property string debugPanelSimulationPanelTestInfoText: qsTr("Test info")
	readonly property string debugPanelSimulationPanelTestWarningText: qsTr("Test warning")
	readonly property string debugPanelSimulationPanelTestErrorText: qsTr("Test error")
	readonly property string debugPanelSimulationPanelClearAllButtonText: qsTr("Clear all")
	readonly property string debugPanelSimulationPanelClearInactiveButtonText: qsTr("Clear inactive")
	readonly property string debugPanelSimulationPanelInactivateTopButtonText: qsTr("Inactivate top")
	readonly property string debugPanelSimulationPanelTotalCountLabelText: qsTr("Total count: %1")
	readonly property string debugPanelSimulationPanelActiveCountLabelText: qsTr("Active count: %1")
	readonly property string debugPanelSimulationPanelHasActiveLabelText: qsTr("Has active: %1")
	readonly property string debugPanelSimulationPanelIsVisibleLabelText: qsTr("Is visible: %1")

	// Setup panel
	readonly property string setupPanelWelcomeTitleText: qsTr("Welcome")
	readonly property string setupPanelWelcomeDescriptionText: qsTr("It's about time you showed up! Let's get things ticking, just follow the steps on screen.")
	readonly property string setupPanelWelcomeButtonText: qsTr("Next")
	readonly property string setupPanelRegisterTitleText: qsTr("Register clock")
	readonly property string setupPanelRegisterDescriptionText: qsTr("Tap the value to edit it, then use the dial below to adjust the value.")
	readonly property string setupPanelAppEnableDescriptionText: qsTr("Use the toggle to turn this application on or off.")
	readonly property string setupPanelAppDateTimePickerTitleText: qsTr("Configure %1")
	readonly property string setupPanelAppDateTimePickerDescriptionText: qsTr("Tap a part of the date or time to edit it, then use the dial below to adjust the value.")
	readonly property string setupPanelAppBackgroundTitleText: qsTr("%1 background")
	readonly property string setupPanelAppBackgroundDescriptionText: qsTr("Select a background below.")
	readonly property string setupPanelAppBackgroundButtonText: qsTr("Next")
	readonly property string setupPanelAppOpacityTitleText: qsTr("%1 opacity")
	readonly property string setupPanelAppOpacityDescriptionText: qsTr("Use the dial below to adjust the background opacity.")
	readonly property string setupPanelAppOpacityButtonText: qsTr("Next")
	readonly property string setupPanelAppBaseColorTitleText: qsTr("%1 base color")
	readonly property string setupPanelAppBaseColorDescriptionText: qsTr("Select a base color below.")
	readonly property string setupPanelAppBaseColorButtonText: qsTr("Next")
	readonly property string setupPanelAppAccentColorTitleText: qsTr("%1 accent color")
	readonly property string setupPanelAppAccentColorDescriptionText: qsTr("Select an accent color below.")
	readonly property string setupPanelAppAccentColorButtonText: qsTr("Next")
	readonly property string setupPanelFinishTitleText: qsTr("All set!")
	readonly property string setupPanelFinishDescriptionText: qsTr("Everything's ticking like clockwork. You're good to go!")
	readonly property string setupPanelFinishButtonText: qsTr("Let's go!")

	// Round progress bar panel
	readonly property string roundProgressBarPanelYearsLabel: qsTr("jaar")
	readonly property string roundProgressBarPanelWeeksLabel: qsTr("weken")
	readonly property string roundProgressBarPanelNotSetup: qsTr("Timer not set up yet")

	// Date display panel
	readonly property string dateDisplayPanelMonday: qsTr("maandag")
	readonly property string dateDisplayPanelTuesday: qsTr("dinsdag")
	readonly property string dateDisplayPanelWednesday: qsTr("woensdag")
	readonly property string dateDisplayPanelThursday: qsTr("donderdag")
	readonly property string dateDisplayPanelFriday: qsTr("vrijdag")
	readonly property string dateDisplayPanelSaturday: qsTr("zaterdag")
	readonly property string dateDisplayPanelSunday: qsTr("zondag")
	readonly property string dateDisplayPanelJanuary: qsTr("januari")
	readonly property string dateDisplayPanelFebruary: qsTr("februari")
	readonly property string dateDisplayPanelMarch: qsTr("maart")
	readonly property string dateDisplayPanelApril: qsTr("april")
	readonly property string dateDisplayPanelMay: qsTr("mei")
	readonly property string dateDisplayPanelJune: qsTr("juni")
	readonly property string dateDisplayPanelJuly: qsTr("juli")
	readonly property string dateDisplayPanelAugust: qsTr("augustus")
	readonly property string dateDisplayPanelSeptember: qsTr("september")
	readonly property string dateDisplayPanelOctober: qsTr("oktober")
	readonly property string dateDisplayPanelNovember: qsTr("november")
	readonly property string dateDisplayPanelDecember: qsTr("december")

	// Seven segment panel
	readonly property string sevenSegmentPanelNotSetup: qsTr("Timer not set up yet")

	// Startup panel
	readonly property string startupPanelLabelText: qsTr("Winding up the gears...")

	// Loading panel
	readonly property string loadingPanelLabelText: qsTr("Recalibrating the clockwork...")

	// Customize dialog
	readonly property string customizeDialogScanQrCodeText: qsTr("Scan to customize clock")
	readonly property string customizeDialogNoNetworkText: qsTr("Connect to a network to customize the clock.")

	// Power off dialog
	readonly property string powerOffDialogHoldText: qsTr("Hold to power off")
	readonly property string powerOffDialogShuttingDownText: qsTr("Powering off...")

	// Watchfaces panel
	readonly property string watchfacesPanelNotSetup: qsTr("System not configured")
}