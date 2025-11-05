import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Bee as Bee
import Components

Item {
    id: inspectorPanel
    property string detailsText: ""

    width: parent.width
    height: parent.height

    QtObject {
        id: mapState

        property QtObject currentMapInterface: null
        property string currentMapPropertyName: ""
    }

    PanelContainer {
        anchors.fill: parent
        currentIndex: {
            if (showModelButton.currentModel)
                return indexOfPanel(modelPanel)
            if (mapState.currentMapInterface)
                return indexOfPanel(mapPanel)
            return indexOfPanel(propertiesPanel)
        }

        Panel {
            id: propertiesPanel

            Button {
                id: showModelButton

                property QtObject currentModel: null

                anchors.top: interfaceSelectorComboBox.top
                anchors.left: parent.left

                text: Translation.debugPanelInspectorShowModelText

                visible: Utils.isQmlSupportedModel(propertiesListView.currentInterface)

                onClicked: {
                    currentModel = propertiesListView.currentInterface
                }
            }

            ComboBox {
                id: interfaceSelectorComboBox

                property int currentInterfaceIndex: -1
                property var availableInterfaces: Utils.getAvailableInterfaces()

                property bool valid: currentInterfaceIndex >= 0 && currentInterfaceIndex < availableInterfaces.length
                property string currentInterfaceName: valid ? availableInterfaces[currentInterfaceIndex].name : ""
                property QtObject currentInterface: valid ? availableInterfaces[currentInterfaceIndex].interface : null

                anchors.horizontalCenter: parent.horizontalCenter
                width: Value.defaultWidthRatio(parent)

                model: availableInterfaces
                textRole: "name"
                valueRole: "interface"

                displayText: currentInterface ? ("" + currentInterface) : "Select Interface"

                onCurrentIndexChanged: {
                    currentInterfaceIndex = currentIndex
                    propertiesListView.overrideInterface = null
                }

                delegate: ItemDelegate {
                    width: interfaceSelectorComboBox.width
                    contentItem: Text {
                        text: modelData.name
                        color: "black"
                        font: interfaceSelectorComboBox.font
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: interfaceSelectorComboBox.highlightedIndex === index
                }
            }

            Button {
                anchors.top: interfaceSelectorComboBox.top
                anchors.right: parent.right

                text: Translation.debugPanelInspectorBackText

                visible: !!propertiesListView.overrideInterface

                onClicked: propertiesListView.overrideInterface = null
            }

            ListView {
                id: propertiesListView

                property QtObject currentInterface: overrideInterface ? overrideInterface : interfaceSelectorComboBox.currentInterface
                property QtObject overrideInterface

                anchors.top: interfaceSelectorComboBox.bottom
                anchors.left: parent.left
                anchors.right: propertiesScrollBar.left
                anchors.bottom: parent.bottom
                anchors.margins: Value.defaultMargin

                model: Utils.properties(currentInterface)

                delegate: Item {
                    anchors.left: parent?.left
                    anchors.right: parent?.right
                    height: nameLabel.height + Value.smallMargin * 2

                    property var propInfo: modelData
                    property bool isPointer: Utils.isPointerProperty(propInfo)
                    property bool isMap: Utils.isMapPropertyType(propInfo)
                    property QtObject subInterface: (isPointer && !isMap) ? Utils.getPointerObject(propertiesListView.currentInterface, propInfo.name) : null

                    MouseArea {
                        id: mouseArea

                        enabled: (isPointer && subInterface) || isMap
                        anchors.left: parent.horizontalCenter
                        anchors.right: valueLabel.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        onClicked: {
                            if (isMap) {
                                mapState.currentMapInterface = propertiesListView.currentInterface
                                mapState.currentMapPropertyName = propInfo.name
                            } else {
                                propertiesListView.overrideInterface = subInterface
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 1
                            visible: mouseArea.enabled
                            color: mouseArea.pressed ? Color.lightGray : Color.lightGray
                            radius: 4
                            border.width: 1
                            border.color: Color.gray
                        }
                    }

                    Text {
                        id: nameLabel

                        anchors.right: parent.horizontalCenter
                        anchors.rightMargin: Value.defaultMargin / 2
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                        text: propInfo.name + " (" + propInfo.typeName + ")"
                        color: propInfo.isWritable ? "black" : Color.gray
                    }

                    Text {
                        id: valueLabel
                        anchors.left: parent.horizontalCenter
                        anchors.leftMargin: Value.defaultMargin / 2
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        fontSizeMode: Text.Fit

                        Component.onCompleted: {
                            var formatValue = function(value, propInfo) {
                                if (value === null) {
                                    return "null"
                                } else if (value === undefined) {
                                    return "undefined"
                                } else if (value === "") {
                                    return "\"\""
                                } else if (propInfo.isEnumType) {
                                    return Utils.enumValueName(propertiesListView.currentInterface, propInfo.name, propertiesListView.currentInterface[propInfo.name])
                                } else {
                                    return "" + value
                                }
                            }

                            if (propInfo.hasNotifySignal) {
                                text = Qt.binding(function() {
                                    return formatValue(propertiesListView.currentInterface[propInfo.name], propInfo)
                                })
                            } else {
                                text = formatValue(propertiesListView.currentInterface[propInfo.name], propInfo)
                            }
                        }
                    }
                }
            }

            ScrollBar {
                id: propertiesScrollBar

                orientation: Qt.Vertical
                policy: ScrollBar.AsNeeded
                anchors.right: parent.right
                anchors.rightMargin: Value.defaultMargin
                anchors.top: propertiesListView.top
                anchors.bottom: propertiesListView.bottom

                contentItem: Rectangle {
                    implicitWidth: 6
                    implicitHeight: 100
                    radius: width / 2
                    color: propertiesScrollBar.pressed ? Color.darkGray : Color.lightGray
                }
            }

            // Bind ScrollBar to ListView
            Binding {
                target: propertiesScrollBar
                property: "active"
                value: propertiesListView.moving || propertiesScrollBar.pressed
            }
            Binding {
                target: propertiesScrollBar
                property: "position"
                value: propertiesListView.visibleArea.yPosition
            }
            Binding {
                target: propertiesScrollBar
                property: "size"
                value: propertiesListView.visibleArea.heightRatio
            }
        }

        Panel {
            id: modelPanel

            Button {
                id: hideModelButton

                anchors.top: parent.top
                anchors.right: parent.right

                text: Translation.debugPanelInspectorBackText

                onClicked: showModelButton.currentModel = null
            }

            ListView {
                id: modelListView

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: hideModelButton.left
                anchors.bottom: parent.bottom
                anchors.rightMargin: Value.defaultMargin

                model: showModelButton.currentModel

                header: Item {
                    anchors.left: parent?.left
                    anchors.right: parent?.right
                    height: Value.smallButtonHeight

                    RowLayout {
                        anchors.fill: parent
                        spacing: Value.smallMargin

                        Repeater {
                            model: modelListView.roles
                            delegate: Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                implicitWidth: 1
                                implicitHeight: 1

                                Text {
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    text: modelData
                                    font.bold: true
                                    fontSizeMode: Text.Fit
                                }
                            }
                        }
                    }
                }

                property var roles: Utils.roleNames(model)

                delegate: Item {
                    id: delegate

                    width: ListView.view.width
                    height: Value.smallButtonHeight

                    property QtObject delegateModel: model

                    RowLayout {
                        anchors.fill: parent
                        spacing: Value.smallMargin

                        Repeater {
                            model: modelListView.roles
                            delegate: Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                implicitWidth: 1
                                implicitHeight: 1

                                Text {
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    text: {
                                        var value = Utils.modelData(modelListView.model, delegate.DelegateModel.itemsIndex, modelData)
                                        var typeInfo = Utils.propertyInformation(delegate.delegateModel, modelData)
                                        return value + "\n" + (typeInfo.variantTypeName || typeInfo.typeName || "")
                                    }
                                    fontSizeMode: Text.Fit
                                    wrapMode: Text.Wrap
                                }
                            }
                        }
                    }
                }
            }

            ScrollBar {
                id: modelScrollBar

                orientation: Qt.Vertical
                policy: ScrollBar.AsNeeded
                anchors.right: parent.right
                anchors.rightMargin: Value.defaultMargin
                anchors.verticalCenter: parent.verticalCenter

                contentItem: Rectangle {
                    implicitWidth: 6
                    implicitHeight: 100
                    radius: width / 2
                    color: modelScrollBar.pressed ? Color.darkGray : Color.lightGray
                }
            }

            // Bind ScrollBar to ListView
            Binding {
                target: modelScrollBar
                property: "active"
                value: modelListView.moving || modelScrollBar.pressed
            }
            Binding {
                target: modelScrollBar
                property: "position"
                value: modelListView.visibleArea.yPosition
            }
            Binding {
                target: modelScrollBar
                property: "size"
                value: modelListView.visibleArea.heightRatio
            }
        }

        Panel {
            id: mapPanel

            property string currentMapPropertyName: mapState.currentMapPropertyName
            property QtObject currentMapInterface: mapState.currentMapInterface
            property var currentEntries: currentMapInterface && currentMapPropertyName
                ? Utils.mapEntries(currentMapInterface, currentMapPropertyName)
                : []

            Button {
                id: hideMapButton

                anchors.top: parent.top
                anchors.right: parent.right

                text: Translation.debugPanelInspectorBackText

                onClicked: {
                    mapState.currentMapInterface = null
                    mapState.currentMapPropertyName = ""
                }
            }

            ComboBox {
                id: mapPropertySelectorComboBox

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: hideMapButton.left
                anchors.rightMargin: Value.defaultMargin

                property var mapProperties: {
                    var result = []
                    if (!mapPanel.currentMapInterface) return result
                    var props = Utils.properties(mapPanel.currentMapInterface)
                    for (var i = 0; i < props.length; i++) {
                        if (Utils.isMapPropertyType(props[i])) {
                            result.push(props[i].name)
                        }
                    }
                    return result
                }

                model: mapProperties

                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < mapProperties.length) {
                        mapState.currentMapPropertyName = mapProperties[currentIndex]
                    }
                }

                delegate: ItemDelegate {
                    width: mapPropertySelectorComboBox.width
                    contentItem: Text {
                        text: modelData
                        color: "black"
                        font: mapPropertySelectorComboBox.font
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: mapPropertySelectorComboBox.highlightedIndex === index
                }
            }

            ListView {
                id: mapListView

                anchors.top: mapPropertySelectorComboBox.bottom
                anchors.left: parent.left
                anchors.right: mapScrollBar.left
                anchors.bottom: parent.bottom
                anchors.margins: Value.defaultMargin

                model: mapPanel.currentEntries

                header: Item {
                    anchors.left: parent?.left
                    anchors.right: parent?.right
                    height: Value.smallButtonHeight

                    RowLayout {
                        anchors.fill: parent
                        spacing: Value.smallMargin

                        Text {
                            Layout.preferredWidth: parent.width / 2
                            Layout.fillHeight: true
                            horizontalAlignment: Text.AlignHCenter
                            text: "Key"
                            font.bold: true
                        }

                        Text {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            horizontalAlignment: Text.AlignHCenter
                            text: "Value"
                            font.bold: true
                        }
                    }
                }

                delegate: Item {
                    id: mapDelegate

                    width: ListView.view.width
                    height: Value.smallButtonHeight

                    property var entry: modelData

                    RowLayout {
                        anchors.fill: parent
                        spacing: Value.smallMargin

                        Item {
                            Layout.preferredWidth: mapDelegate.width / 2
                            Layout.fillHeight: true

                            Text {
                                anchors.fill: parent
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: mapDelegate.entry.key
                                elide: Text.ElideRight
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            MouseArea {
                                id: mapValueMouseArea

                                enabled: mapDelegate.entry.isPointer
                                anchors.fill: parent

                                onClicked: {
                                    if (mapDelegate.entry.isPointer && mapDelegate.entry.value) {
                                        mapState.currentMapInterface = null
                                        mapState.currentMapPropertyName = ""
                                        propertiesListView.overrideInterface = mapDelegate.entry.value
                                    }
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    visible: mapValueMouseArea.enabled
                                    color: mapValueMouseArea.pressed ? Color.lightGray : Color.lightGray
                                    radius: 4
                                    border.width: 1
                                    border.color: Color.gray
                                }
                            }

                            Text {
                                anchors.fill: parent
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: mapDelegate.entry.displayValue
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }

            ScrollBar {
                id: mapScrollBar

                orientation: Qt.Vertical
                policy: ScrollBar.AsNeeded
                anchors.right: parent.right
                anchors.rightMargin: Value.defaultMargin
                anchors.top: mapListView.top
                anchors.bottom: mapListView.bottom

                contentItem: Rectangle {
                    implicitWidth: 6
                    implicitHeight: 100
                    radius: width / 2
                    color: mapScrollBar.pressed ? Color.darkGray : Color.lightGray
                }
            }

            // Bind ScrollBar to ListView
            Binding {
                target: mapScrollBar
                property: "active"
                value: mapListView.moving || mapScrollBar.pressed
            }
            Binding {
                target: mapScrollBar
                property: "position"
                value: mapListView.visibleArea.yPosition
            }
            Binding {
                target: mapScrollBar
                property: "size"
                value: mapListView.visibleArea.heightRatio
            }
        }
    }
}
