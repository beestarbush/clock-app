qt_add_library(PanelLib STATIC)

set_target_properties(PanelLib PROPERTIES AUTOMOC ON)
target_link_libraries(PanelLib PRIVATE Qt6::Quick)

qt_add_qml_module(PanelLib
    URI Panels
    VERSION 1.0
    QML_FILES
        ClockPanel.qml
        CountdownPanel.qml
        DebugPanel.qml
        Debug/GalleryPanel.qml
        Debug/InspectorPanel.qml
        Debug/LoggingPanel.qml
        Debug/SimulationPanel.qml
        Debug/UtilityPanel.qml
        LoadingPanel.qml
        LowerMainPanel.qml
        LowerMenuOverlay.qml
        PendulumPanel.qml
        RoundProgressBarPanel.qml
        SetupPanel.qml
        Setup/DateTimePickerPanel.qml
        Setup/IntegerSelectPanel.qml
        Setup/TitlePanel.qml
        Setup/ToggleButtonPanel.qml
        SevenSegmentPanel.qml
        StartupPanel.qml
        UpperMainPanel.qml
        UpperMenuOverlay.qml
        WatchfacesPanel.qml
    DEPENDENCIES
        Components/1.0
)