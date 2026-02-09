// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root

    Rectangle {
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                GuiController.closeNotesEditor();
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width * 0.9, Style.fromPixel(500))
            height: Math.min(parent.height * 0.7, Style.fromPixel(600))
            color: "#131c27"
            border.width: Style.fromPixel(1)
            border.color: "#222b38"
            radius: Style.fromPixel(12)

            ColumnLayout {
                id: contentLayout
                anchors.fill: parent
                anchors.margins: Style.fromPixel(24)
                spacing: Style.fromPixel(16)

                Text {
                    text: "Notes"
                    font.pixelSize: Style.fromPixel(24)
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: GuiController.notesTaskTitle
                    font.pixelSize: Style.fromPixel(14)
                    color: "#aaaaaa"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#1a2332"
                    border.width: Style.fromPixel(1)
                    border.color: "#2a3342"
                    radius: Style.fromPixel(8)

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: Style.fromPixel(8)
                        clip: true

                        TextArea {
                            id: notesTextArea
                            text: GuiController.notesText
                            font.pixelSize: Style.fromPixel(14)
                            color: "white"
                            wrapMode: TextArea.Wrap
                            selectByMouse: true
                            background: Rectangle {
                                color: "transparent"
                            }
                            Component.onCompleted: {
                                notesTextArea.forceActiveFocus();
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Style.fromPixel(12)

                    Button {
                        text: "Cancel"
                        font.pixelSize: Style.fromPixel(16)
                        background: Rectangle {
                            color: "#2a3342"
                            radius: Style.fromPixel(8)
                        }
                        contentItem: Text {
                            color: "white"
                            text: "Cancel"
                            font.pixelSize: Style.fromPixel(16)
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(48)
                        onClicked: {
                            GuiController.closeNotesEditor();
                        }
                    }

                    Button {
                        text: "Save"
                        font.pixelSize: Style.fromPixel(16)
                        background: Rectangle {
                            color: "#1884f7"
                            radius: Style.fromPixel(8)
                        }
                        contentItem: Text {
                            color: "white"
                            text: "Save"
                            font.pixelSize: Style.fromPixel(16)
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(48)
                        onClicked: {
                            GuiController.saveNotes(notesTextArea.text);
                        }
                    }
                }
            }
        }
    }
}
