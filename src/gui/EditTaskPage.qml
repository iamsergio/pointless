// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Page {
    id: root
    objectName: "editTask"
    pageTitle: "Edit Task"
    rightButtonIcon: "\uf00c"
    rightButtonObjectName: "saveButton"

    signal saveClicked(string title, string tag, bool evening)

    property string selectedTag: ""
    property bool isEvening: false

    onVisibleChanged: {
        if (visible) {
            titleInput.text = GuiController.titleInEditor;
            titleInput.forceActiveFocus();
            root.selectedTag = GuiController.tagInEditor;
            root.isEvening = GuiController.isEveningInEditor;
        }
    }

    onRightButtonClicked: root.saveClicked(titleInput.text, root.selectedTag, root.isEvening)

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.fromPixel(20)

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
