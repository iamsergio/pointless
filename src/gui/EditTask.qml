// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    color: Style.background
    objectName: "editTask"

    signal backClicked
    signal saveClicked(string title, string tag, bool evening)

    property string selectedTag: ""
    property bool isEvening: false

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(16)
        spacing: Style.fromPixel(20)

        onVisibleChanged: {
            if (visible) {
                titleInput.text = GuiController.titleInEditor;
                titleInput.forceActiveFocus();
                root.selectedTag = GuiController.tagInEditor;
                root.isEvening = GuiController.isEveningInEditor;
            }
        }

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(10)

            FontAwesomeButton {
                id: backButton
                fontAwesomeIcon: "\uf060" // Arrow left
                backgroundColor: "transparent"
                iconColor: "white"
                onClicked: root.backClicked()
            }

            Text {
                text: "Edit Task"
                color: "white"
                font.pixelSize: Style.fromPixel(18)
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            FontAwesomeButton {
                id: saveButton
                objectName: "saveButton"
                fontAwesomeIcon: "\uf00c" // Check
                backgroundColor: "transparent"
                iconColor: "white"
                onClicked: root.saveClicked(titleInput.text, root.selectedTag, root.isEvening)
            }
        }

        // Title Section
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(5)

            Text {
                text: "Title"
                color: Style.sectionTextColor
                font.pixelSize: Style.fromPixel(14)
                font.bold: true
            }

            LineEdit {
                id: titleInput
                objectName: "titleInput"
                focus: true
                Layout.fillWidth: true
                onTextEntered: function (text) {
                    root.saveClicked(text, root.selectedTag, root.isEvening);
                }
            }
        }

        // Tag Section
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(5)

            Text {
                text: "Tag"
                color: Style.sectionTextColor
                font.pixelSize: Style.fromPixel(14)
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: tagFlow.implicitHeight + Style.fromPixel(20)
                color: Style.taskBackground
                radius: 4

                Flow {
                    id: tagFlow
                    anchors.fill: parent
                    anchors.margins: Style.fromPixel(10)
                    spacing: Style.fromPixel(8)

                    Repeater {
                        model: GuiController.tagFilterModel
                        Tag {
                            required property string name
                            objectName: "tag_" + name
                            isInteractive: true
                            isSelected: root.selectedTag === name
                            tagName: name
                            onClicked: function (name) {
                                root.selectedTag = name;
                            }
                        }
                    }
                }
            }
        }

        // Evening Section
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(5)

            CheckBox {
                id: eveningCheckBox
                objectName: "eveningCheckBox"
                checked: root.isEvening
                onCheckedChanged: root.isEvening = checked
            }

            Text {
                text: "Evening"
                color: Style.sectionTextColor
                font.pixelSize: Style.fromPixel(14)
                font.bold: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: eveningCheckBox.toggle()
                }
            }
        }

        // Date Section
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Style.fromPixel(5)

            Text {
                text: "Date"
                color: Style.sectionTextColor
                font.pixelSize: Style.fromPixel(14)
                font.bold: true
            }

            Calendar {
                id: calendar
                currentDate: GuiController.dateInEditor
                Layout.fillWidth: true
                Layout.fillHeight: true
                onDateSelected: function (selectedDate) {
                    GuiController.setDateInEditor(selectedDate);
                }
            }
        }
    }
}
