// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root
    visible: GuiController.newTagPopupVisible

    function open() {
        tagNameField.text = "";
        GuiController.newTagPopupVisible = true;
        tagNameField.forceActiveFocus();
    }

    function close() {
        GuiController.newTagPopupVisible = false;
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
            height: Style.fromPixel(220)
            color: "#131c27"
            border.width: Style.fromPixel(1)
            border.color: "#222b38"
            radius: Style.fromPixel(12)

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Style.fromPixel(24)
                spacing: Style.fromPixel(16)

                Text {
                    text: "New Tag"
                    font.pixelSize: Style.fromPixel(24)
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }

                TextField {
                    id: tagNameField
                    Layout.fillWidth: true
                    placeholderText: "Tag name"
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
                        if (tagNameField.text.trim().length > 0) {
                            GuiController.addTag(tagNameField.text.trim());
                            root.close();
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
                        text: "Add"
                        font.pixelSize: Style.fromPixel(16)
                        background: Rectangle {
                            color: "#1884f7"
                            radius: Style.fromPixel(8)
                        }
                        contentItem: Text {
                            color: "white"
                            text: "Add"
                            font.pixelSize: Style.fromPixel(16)
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(48)
                        onClicked: {
                            if (tagNameField.text.trim().length > 0) {
                                GuiController.addTag(tagNameField.text.trim());
                                root.close();
                            }
                        }
                    }
                }
            }
        }
    }
}
