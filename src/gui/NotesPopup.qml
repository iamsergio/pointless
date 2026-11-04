// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root

    Rectangle {
        color: "#8C000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            // qmllint disable incompatible-type
            readonly property InputMethod inputMethod: Qt.inputMethod
            onClicked: {
                if (inputMethod.visible) {
                    inputMethod.hide();
                } else {
                    GuiController.closeNotesEditor();
                }
            }
        }

        Rectangle {
            anchors {
                top: parent.top
                topMargin: Style.fromPixel(12)
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Style.fromPixel(GuiController.isMobile ? 32 : 16) + Style.fromPixel(60) + Style.fromPixel(14)
            }
            width: Math.min(parent.width - Style.fromPixel(24), Style.fromPixel(560))
            color: Style.surface
            radius: Style.fromPixel(28)

            MouseArea {
                anchors.fill: parent
            }

            ColumnLayout {
                id: contentLayout
                anchors.fill: parent
                anchors.margins: Style.fromPixel(18)
                spacing: Style.fromPixel(12)

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Cancel"
                        font.pixelSize: Style.fromPixel(15)
                        color: Style.inkSecondary
                        Layout.preferredWidth: Style.fromPixel(70)

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -Style.fromPixel(8)
                            onClicked: GuiController.closeNotesEditor()
                        }
                    }

                    Text {
                        text: "Notes"
                        font.pixelSize: Style.fromPixel(16)
                        font.weight: Font.DemiBold
                        color: Style.ink
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Item {
                        Layout.preferredWidth: Style.fromPixel(70)
                        implicitHeight: Style.fromPixel(34)

                        Rectangle {
                            anchors.right: parent.right
                            width: Style.fromPixel(64)
                            height: parent.height
                            radius: height / 2
                            color: Style.accent

                            Text {
                                anchors.centerIn: parent
                                text: "Save"
                                font.pixelSize: Style.fromPixel(14.5)
                                font.weight: Font.DemiBold
                                color: Style.accentInk
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: GuiController.saveNotes(notesTextArea.text)
                            }
                        }
                    }
                }

                Text {
                    text: GuiController.notesTaskTitle
                    font.pixelSize: Style.fromPixel(14)
                    color: Style.inkSecondary
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Style.raised
                    radius: Style.fromPixel(16)

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: Style.fromPixel(8)
                        clip: true

                        TextArea {
                            id: notesTextArea
                            text: GuiController.notesText
                            font.pixelSize: Style.fromPixel(15)
                            color: Style.ink
                            wrapMode: TextArea.Wrap
                            selectByMouse: true
                            background: null
                            Component.onCompleted: {
                                notesTextArea.forceActiveFocus();
                            }
                        }
                    }
                }
            }
        }
    }
}
