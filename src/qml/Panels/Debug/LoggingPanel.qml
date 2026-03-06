import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Components
import Bee as Backend

Rectangle {
    id: loggingPanel

    color: Color.lightGray

    property string selectedCategory: "All"
    property string selectedPriority: "All"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

            Text {
                Layout.fillWidth: true
                text: "Application Log Viewer"
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: "Priority:"
                font.pixelSize: 11
            }

            ComboBox {
                id: priorityFilter
                Layout.preferredWidth: 120

                model: ["All", "debug", "info", "warning", "critical", "fatal"]

                onCurrentTextChanged: {
                    loggingPanel.selectedPriority = currentText;
                }

                Component.onCompleted: {
                    currentIndex = 0; // Default to "All"
                }
            }

            Text {
                text: "Category:"
                font.pixelSize: 11
            }

            ComboBox {
                id: categoryFilter
                Layout.preferredWidth: 150

                model: {
                    var categories = ["All"];
                    var allCategories = Backend.Services.logging.model.getCategories();
                    return categories.concat(allCategories);
                }

                onCurrentTextChanged: {
                    loggingPanel.selectedCategory = currentText;
                }

                Component.onCompleted: {
                    currentIndex = 0; // Default to "All"
                }
            }

            Button {
                text: "Clear"
                onClicked: Backend.Services.logging.model.clearAll()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#cccccc"
            border.width: 1

            ListView {
                id: logListView
                anchors.fill: parent
                anchors.margins: 5
                clip: true

                model: Backend.Services.logging.model

                delegate: Rectangle {
                    width: logListView.width
                    height: visible ? (contentLayout.implicitHeight + 10) : 0
                    visible: {
                        var categoryMatch = loggingPanel.selectedCategory === "All" || category === loggingPanel.selectedCategory;
                        var priorityMatch = loggingPanel.selectedPriority === "All" || priority === loggingPanel.selectedPriority;
                        return categoryMatch && priorityMatch;
                    }
                    color: {
                        if (priority === "critical" || priority === "fatal") return "#ffe6e6";
                        if (priority === "warning") return "#fff9e6";
                        return index % 2 === 0 ? "#f9f9f9" : "#ffffff";
                    }
                    border.color: "#eeeeee"
                    border.width: 1

                    ColumnLayout {
                        id: contentLayout
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 3

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Text {
                                text: {
                                    const date = new Date(timestamp);
                                    return date.toLocaleTimeString(Qt.locale(), "hh:mm:ss.zzz");
                                }
                                font.pixelSize: 11
                                color: "#666666"
                                Layout.preferredWidth: 100
                            }

                            Rectangle {
                                Layout.preferredWidth: 60
                                Layout.preferredHeight: 18
                                color: {
                                    if (priority === "debug") return "#e0e0e0";
                                    if (priority === "info") return "#d4edda";
                                    if (priority === "warning") return "#fff3cd";
                                    if (priority === "critical") return "#f8d7da";
                                    if (priority === "fatal") return "#dc3545";
                                    return "#f0f0f0";
                                }
                                radius: 3

                                Text {
                                    anchors.centerIn: parent
                                    text: priority.toUpperCase()
                                    font.pixelSize: 9
                                    font.bold: true
                                    color: {
                                        if (priority === "fatal") return "#ffffff";
                                        return "#000000";
                                    }
                                }
                            }

                            Text {
                                text: category
                                font.pixelSize: 10
                                font.bold: true
                                color: {
                                    if (category === "websocket") return "#0066cc";
                                    if (category === "default") return "#666666";
                                    return "#009900";
                                }
                                Layout.preferredWidth: 150
                            }

                            Text {
                                text: {
                                    // Extract direction from websocket messages
                                    if (message.startsWith("→")) return "→";
                                    if (message.startsWith("←")) return "←";
                                    return "•";
                                }
                                font.pixelSize: 11
                                font.bold: true
                                color: message.startsWith("→") ? "#0066cc" : "#009900"
                                Layout.preferredWidth: 20
                                visible: category === "websocket"
                            }

                            Text {
                                Layout.fillWidth: true
                                text: {
                                    // Remove direction prefix for websocket messages
                                    if (category === "websocket") {
                                        return message.replace(/^[→←]\s+\w+\s+/, "");
                                    }
                                    return message;
                                }
                                font.pixelSize: 10
                                font.family: category === "websocket" ? "Courier" : "Sans"
                                color: "#000000"
                                elide: Text.ElideRight
                                wrapMode: Text.WordWrap
                                maximumLineCount: 2
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            detailPopup.messageCategory = category;
                            detailPopup.messagePriority = priority;
                            detailPopup.messageText = message;
                            detailPopup.messageTimestamp = timestamp;
                            detailPopup.open();
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}

                onCountChanged: {
                    // Auto-scroll to bottom when new items are added
                    positionViewAtEnd();
                }
            }
        }

        Text {
            Layout.fillWidth: true
            text: {
                var total = Backend.Services.logging.model.count;
                var visible = 0;
                for (var i = 0; i < total; i++) {
                    var item = Backend.Services.logging.model.get(i);
                    if ((loggingPanel.selectedCategory === "All" || item.category === loggingPanel.selectedCategory) &&
                        (loggingPanel.selectedPriority === "All" || item.priority === loggingPanel.selectedPriority)) {
                        visible++;
                    }
                }
                return "Showing " + visible + " of " + total + " entries";
            }
            font.pixelSize: 11
            color: "#666666"
        }
    }

    Connections {
        target: Backend.Services.logging.model
        function onCountChanged() {
            // Refresh category list when new categories appear
            var currentSelection = categoryFilter.currentText;
            var categories = ["All"];
            var allCategories = Backend.Services.logging.model.getCategories();
            categoryFilter.model = categories.concat(allCategories);
            
            // Restore selection if it still exists
            var idx = categoryFilter.find(currentSelection);
            if (idx >= 0) {
                categoryFilter.currentIndex = idx;
            }
        }
    }

    // Popup for detailed message view
    Popup {
        id: detailPopup
        width: parent.width * 0.8
        height: parent.height * 0.6
        anchors.centerIn: parent

        property string messageCategory: ""
        property string messagePriority: ""
        property string messageText: ""
        property real messageTimestamp: 0

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    text: "Message Details"
                    font.pixelSize: 14
                    font.bold: true
                    Layout.fillWidth: true
                }

                Rectangle {
                    Layout.preferredWidth: 70
                    Layout.preferredHeight: 22
                    color: {
                        if (detailPopup.messagePriority === "debug") return "#e0e0e0";
                        if (detailPopup.messagePriority === "info") return "#d4edda";
                        if (detailPopup.messagePriority === "warning") return "#fff3cd";
                        if (detailPopup.messagePriority === "critical") return "#f8d7da";
                        if (detailPopup.messagePriority === "fatal") return "#dc3545";
                        return "#f0f0f0";
                    }
                    radius: 3

                    Text {
                        anchors.centerIn: parent
                        text: detailPopup.messagePriority.toUpperCase()
                        font.pixelSize: 10
                        font.bold: true
                        color: {
                            if (detailPopup.messagePriority === "fatal") return "#ffffff";
                            return "#000000";
                        }
                    }
                }

                Text {
                    text: detailPopup.messageCategory
                    font.pixelSize: 11
                    font.bold: true
                    color: "#0066cc"
                }

                Text {
                    text: {
                        const date = new Date(detailPopup.messageTimestamp);
                        return date.toLocaleString(Qt.locale());
                    }
                    font.pixelSize: 10
                    color: "#666666"
                }
            }

            TextArea {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: detailPopup.messageText
                readOnly: true
                font.family: "Courier"
                font.pixelSize: 10
                wrapMode: TextArea.WordWrap
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: "Close"
                onClicked: detailPopup.close()
            }
        }
    }
}