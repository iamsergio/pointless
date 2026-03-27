// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root
    visible: GuiController.renameTagPopupVisible

    onVisibleChanged: {
        if (visible) {
            newNameField.text = GuiController.renameTagOldName;
            newNameField.forceActiveFocus();
            newNameField.selectAll();
        }
    }

    function close() {
        GuiController.renameTagPopupVisible = false;
    }

    Rectangle {
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.close();
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width * 0.9, Style.fromPixel(400))
            height: Style.fromPixel(270)
            color: "#131c27"
            border.width: Style.fromPixel(1)
            border.color: "#222b38"
            radius: Style.fromPixel(12)

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Style.fromPixel(24)
                spacing: Style.fromPixel(16)

                Text {
                    text: "Rename Tag"
                    font.pixelSize: Style.fromPixel(24)
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: GuiController.renameTagOldName
                    font.pixelSize: Style.fromPixel(14)
                    color: "#888888"
                    Layout.fillWidth: true
                }

                TextField {
                    id: newNameField
                    Layout.fillWidth: true
                    placeholderText: "New tag name"
                    placeholderTextColor: "#888888"
                    font.pixelSize: Style.fromPixel(16)
                    color: "white"
                    background: Rectangle {
                        color: "#1a2332"
                        border.width: Style.fromPixel(1)
                        border.color: "#2a3342"
                        radius: Style.fromPixel(8)
                    }
                    Keys.onReturnPressed: {
                        if (newNameField.text.trim().length > 0) {
                            GuiController.renameTag(GuiController.renameTagOldName, newNameField.text.trim());
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
                            root.close();
                        }
                    }

                    Button {
                        text: "Rename"
                        font.pixelSize: Style.fromPixel(16)
                        background: Rectangle {
                            color: "#1884f7"
                            radius: Style.fromPixel(8)
                        }
                        contentItem: Text {
                            color: "white"
                            text: "Rename"
                            font.pixelSize: Style.fromPixel(16)
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(48)
                        onClicked: {
                            if (newNameField.text.trim().length > 0) {
                                GuiController.renameTag(GuiController.renameTagOldName, newNameField.text.trim());
                            }
                        }
                    }
                }
            }
        }
    }
}
