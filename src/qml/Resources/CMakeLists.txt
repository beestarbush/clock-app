qt_add_library(ResourcesLib STATIC)

set_target_properties(ResourcesLib PROPERTIES AUTOMOC ON)
target_link_libraries(ResourcesLib PRIVATE Qt6::Quick)

qt_add_qml_module(ResourcesLib
    URI Resources
    VERSION 1.0
)

qt_add_resources(ResourcesLib "icons"
	FILES
		Icons/home.svg
		Icons/settings.svg
		Icons/notifications.svg
		Icons/version.svg
		Icons/brightness.svg
		Icons/wizard.svg
)
