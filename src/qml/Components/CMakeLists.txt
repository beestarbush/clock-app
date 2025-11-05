qt_add_library(ComponentLib STATIC)

set_target_properties(ComponentLib PROPERTIES AUTOMOC ON)
target_link_libraries(ComponentLib PRIVATE Qt6::Quick)

set(regular_qml_components
	BehaviorWhenVisible.qml
	Button.qml
	Circle.qml
	Clock.qml
	ColorWheel.qml
	DateTimePicker.qml
	Dialog.qml
	DialogOverlay.qml
	DialWheel.qml
	DynamicPanel.qml
	Flickable.qml
	MediaCarousel.qml
	MenuDialog.qml
	NotificationCarousel.qml
	NotificationOverlay.qml
	Panel.qml
	PanelContainer.qml
	Pendulum.qml
	RingMenu.qml
	RoundButton.qml
	RoundLayout.qml
	RoundPanel.qml
	RoundProgressBar.qml
	SevenSegmentDisplay.qml
	Spacer.qml
	ToggleButton.qml
)

set(qml_singletons
	Color.qml
	PanelTransition.qml
	Translation.qml
	Utils.qml
	Value.qml
)

set_source_files_properties(${qml_singletons}
	PROPERTIES
	QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(ComponentLib
    URI Components
    VERSION 1.0
    QML_FILES
		${regular_qml_components}
		${qml_singletons}
)
