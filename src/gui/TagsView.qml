// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Flickable {
    id: root
    contentWidth: width
    contentHeight: column.implicitHeight + Style.fromPixel(16)
    boundsBehavior: Flickable.StopAtBounds
    clip: true

    ColumnLayout {
        id: column
        width: root.width
        spacing: Style.fromPixel(14)

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: rowsColumn.implicitHeight
            radius: Style.fromPixel(18)
            color: Style.surface
            clip: true

            Column {
                id: rowsColumn
                width: parent.width

                Repeater {
                    id: tagRepeater
                    model: GuiController.tagFilterModel

                    delegate: Item {
                        id: delegateRoot
                        required property int index
                        required property string name
                        required property int taskCount
                        objectName: "tagRow_" + name
                        width: rowsColumn.width
                        height: Style.fromPixel(64)

                        Item {
                            objectName: "tagChip_" + delegateRoot.name
                            anchors {
                                left: parent.left
                                right: renameButton.left
                                top: parent.top
                                bottom: parent.bottom
                            }

                            Rectangle {
                                id: dot
                                x: Style.fromPixel(16)
                                anchors.verticalCenter: parent.verticalCenter
                                width: Style.fromPixel(12)
                                height: width
                                radius: width / 2
                                color: GuiController.colorFromTag(delegateRoot.name)
                            }

                            Column {
                                anchors {
                                    left: dot.right
                                    leftMargin: Style.fromPixel(14)
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                }
                                spacing: Style.fromPixel(2)

                                Text {
                                    width: parent.width
                                    text: delegateRoot.name
                                    color: Style.ink
                                    font.pixelSize: Style.fromPixel(15.5)
                                    elide: Text.ElideRight
                                }

                                Text {
                                    text: delegateRoot.taskCount === 0 ? "No tasks" : (delegateRoot.taskCount === 1 ? "1 task" : delegateRoot.taskCount + " tasks")
                                    color: Style.inkSecondary
                                    font.pixelSize: Style.fromPixel(12.5)
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    GuiController.currentTag = delegateRoot.name;
                                    GuiController.currentPage = "tasksByTag";
                                }
                            }
                        }

                        IconButton {
                            id: renameButton
                            objectName: "renameTag_" + delegateRoot.name
                            anchors {
                                right: deleteButton.left
                                verticalCenter: parent.verticalCenter
                            }
                            diameter: Style.fromPixel(32)
                            fontAwesomeIcon: "\uf304"
                            iconSize: Style.fromPixel(14)
                            onClicked: GuiController.openRenameTagPopup(delegateRoot.name)
                        }

                        IconButton {
                            id: deleteButton
                            anchors {
                                right: parent.right
                                rightMargin: Style.fromPixel(10)
                                verticalCenter: parent.verticalCenter
                            }
                            diameter: Style.fromPixel(32)
                            fontAwesomeIcon: "\uf2ed"
                            iconSize: Style.fromPixel(14)
                            enabled: delegateRoot.taskCount === 0
                            opacity: enabled ? 1.0 : 0.35
                            onClicked: GuiController.removeTag(delegateRoot.name)
                        }

                        Rectangle {
                            visible: delegateRoot.index < tagRepeater.count - 1
                            anchors.bottom: parent.bottom
                            x: Style.fromPixel(16)
                            width: parent.width - Style.fromPixel(16)
                            height: 1
                            color: Style.line
                        }
                    }
                }
            }
        }

        Text {
            Layout.fillWidth: true
            text: "A tag that's still on tasks can't be deleted. Remove it from those tasks first."
            color: Style.inkTertiary
            font.pixelSize: Style.fromPixel(12.5)
            wrapMode: Text.WordWrap
        }
    }
}
