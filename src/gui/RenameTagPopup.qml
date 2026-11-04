// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root
    visible: GuiController.renameTagPopupVisible

    property int taskCount: 0

    onVisibleChanged: {
        if (visible) {
            newNameField.text = GuiController.renameTagOldName;
            Qt.callLater(function () {
                newNameField.forceActiveFocus();
                newNameField.selectAll();
            });
        }
    }

    function close() {
        GuiController.renameTagPopupVisible = false;
    }

    Repeater {
        model: GuiController.tagModel

        Item {
            required property string name
            required property int taskCount

            readonly property bool matches: name === GuiController.renameTagOldName
            readonly property int shownCount: matches ? taskCount : -1

            onShownCountChanged: {
                if (matches)
                    root.taskCount = taskCount;
            }
            onMatchesChanged: {
                if (matches)
                    root.taskCount = taskCount;
            }
        }
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
                    text: "Rename tag"
                    font.pixelSize: Style.fromPixel(21)
                    font.weight: Font.DemiBold
                    color: Style.ink
                }

                Text {
                    text: root.taskCount > 0 ? root.taskCount + (root.taskCount === 1 ? " task uses \u201c" : " tasks use \u201c") + GuiController.renameTagOldName + "\u201d. They keep the tag under its new name." : "Tasks with this tag keep it under the new name."
                    font.pixelSize: Style.fromPixel(14)
                    color: Style.inkSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextField {
                    id: newNameField
                    Layout.fillWidth: true
                    implicitHeight: Style.fromPixel(50)
                    placeholderText: "New tag name"
                    placeholderTextColor: Style.inkTertiary
                    font.pixelSize: Style.fromPixel(16)
                    color: Style.ink
                    leftPadding: Style.fromPixel(16)
                    rightPadding: Style.fromPixel(16)
                    selectByMouse: true
                    background: Rectangle {
                        color: Style.raised
                        radius: Style.fromPixel(14)
                        border.width: newNameField.activeFocus ? 1.5 : 0
                        border.color: Style.accent
                    }
                    Keys.onReturnPressed: {
                            if (newNameField.text.trim().length > 0) {
                                GuiController.renameTag(GuiController.renameTagOldName, newNameField.text.trim());
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
                        id: renameConfirmButton
                        text: "Rename"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        implicitHeight: Style.fromPixel(48)
                        background: Rectangle {
                            color: Style.accent
                            radius: Style.fromPixel(15)
                            opacity: renameConfirmButton.pressed ? 0.8 : 1
                        }
                        contentItem: Text {
                            color: Style.accentInk
                            text: "Rename"
                            font.pixelSize: Style.fromPixel(15.5)
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
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
