// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root
    visible: GuiController.newTagPopupVisible

    onVisibleChanged: {
        if (visible) {
            tagNameField.text = "";
            tagNameField.forceActiveFocus();
        }
    }

    function open() {
        tagNameField.text = "";
        GuiController.newTagPopupVisible = true;
        tagNameField.forceActiveFocus();
    }

    function close() {
        GuiController.newTagPopupVisible = false;
    }

    Rectangle {
        color: "#8C000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.close();
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width - Style.fromPixel(40), Style.fromPixel(400))
            height: contentLayout.implicitHeight + Style.fromPixel(36)
            color: Style.surface
            radius: Style.fromPixel(26)

            MouseArea {
                anchors.fill: parent
            }

            ColumnLayout {
                id: contentLayout
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: Style.fromPixel(22)
                    rightMargin: Style.fromPixel(22)
                }
                spacing: Style.fromPixel(14)

                Text {
                    text: "New tag"
                    font.pixelSize: Style.fromPixel(21)
                    font.weight: Font.DemiBold
                    color: Style.ink
                }

                Text {
                    text: "Tags group related tasks across your views, like work or errands."
                    font.pixelSize: Style.fromPixel(14)
                    color: Style.inkSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: tagNameField
                    Layout.fillWidth: true
                    implicitHeight: Style.fromPixel(50)
                    placeholderText: "Tag name"
                    placeholderTextColor: Style.inkTertiary
                    font.pixelSize: Style.fromPixel(16)
                    color: Style.ink
                    leftPadding: Style.fromPixel(16)
                    rightPadding: Style.fromPixel(16)
                    selectByMouse: true
                    background: Rectangle {
                        color: Style.raised
                        radius: Style.fromPixel(14)
                        border.width: tagNameField.activeFocus ? 1.5 : 0
                        border.color: Style.accent
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
                    spacing: Style.fromPixel(10)

                    Button {
                        id: cancelButton
                        objectName: "popupCancelButton"
                        text: "Cancel"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        implicitHeight: Style.fromPixel(48)
                        background: Rectangle {
                            color: Style.raised
                            radius: Style.fromPixel(15)
                            opacity: cancelButton.pressed ? 0.8 : 1
                        }
                        contentItem: Text {
                            color: Style.ink
                            text: "Cancel"
                            font.pixelSize: Style.fromPixel(15.5)
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            root.close();
                        }
                    }
                    Button {
                        id: addConfirmButton
                        text: "Add tag"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        implicitHeight: Style.fromPixel(48)
                        background: Rectangle {
                            color: Style.accent
                            radius: Style.fromPixel(15)
                            opacity: addConfirmButton.pressed ? 0.8 : 1
                        }
                        contentItem: Text {
                            color: Style.accentInk
                            text: "Add tag"
                            font.pixelSize: Style.fromPixel(15.5)
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
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
