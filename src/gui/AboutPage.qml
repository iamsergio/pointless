// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "about"
    pageTitle: "About"

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    property int numTasks: 0
    property int localRevision: 0

    onVisibleChanged: {
        if (visible) {
            numTasks = GuiController.numTasks();
            localRevision = GuiController.localRevision();
        }
    }

    ColumnLayout {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: Style.fromPixel(10)
        }
        spacing: Style.fromPixel(4)

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Style.fromPixel(68)
            Layout.preferredHeight: Style.fromPixel(68)
            radius: width / 2
            color: Style.accent

            Text {
                anchors.centerIn: parent
                text: "\uf00c"
                font.family: fontAwesome.name
                font.pixelSize: Style.fromPixel(28)
                color: Style.accentInk
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: Style.fromPixel(12)
            text: "Pointless"
            color: Style.ink
            font.pixelSize: Style.fromPixel(30)
            font.weight: Font.Bold
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "A simple to-do manager"
            color: Style.inkSecondary
            font.pixelSize: Style.fromPixel(15)
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: Style.fromPixel(24)
            implicitHeight: infoColumn.implicitHeight
            radius: Style.fromPixel(18)
            color: Style.surface
            clip: true

            component InfoRow: Item {
                id: infoRow
                property string label: ""
                property string value: ""
                property bool showDivider: true

                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(48)

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: Style.fromPixel(16)
                        verticalCenter: parent.verticalCenter
                    }
                    text: infoRow.label
                    color: Style.ink
                    font.pixelSize: Style.fromPixel(15)
                }

                Text {
                    anchors {
                        right: parent.right
                        rightMargin: Style.fromPixel(16)
                        verticalCenter: parent.verticalCenter
                    }
                    text: infoRow.value
                    color: Style.inkSecondary
                    font.pixelSize: Style.fromPixel(15)
                }

                Rectangle {
                    visible: infoRow.showDivider
                    anchors.bottom: parent.bottom
                    x: Style.fromPixel(16)
                    width: parent.width - Style.fromPixel(16)
                    height: 1
                    color: Style.line
                }
            }

            ColumnLayout {
                id: infoColumn
                width: parent.width
                spacing: 0

                InfoRow {
                    label: "Qt version"
                    value: GuiController.qtVersion()
                }

                InfoRow {
                    label: "Local revision"
                    value: root.localRevision
                }

                InfoRow {
                    label: "Tasks"
                    value: root.numTasks
                    showDivider: false
                }
            }
        }
    }
}
