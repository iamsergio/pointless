// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts
import pointless 1.0

Item {
    id: root

    property alias moveItems: moveFlow.data
    property alias actionItems: actionColumn.data

    signal dismissed

    Rectangle {
        anchors.fill: parent
        color: "#8C000000"

        MouseArea {
            anchors.fill: parent
            onClicked: root.dismissed()
        }
    }

    Rectangle {
        id: sheet
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: sheetColumn.implicitHeight + Style.fromPixel(12) + Style.fromPixel(16) + root.SafeArea.margins.bottom
        color: Style.surface
        topLeftRadius: Style.fromPixel(28)
        topRightRadius: Style.fromPixel(28)

        MouseArea {
            anchors.fill: parent
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            y: Style.fromPixel(10)
            width: Style.fromPixel(38)
            height: Style.fromPixel(5)
            radius: height / 2
            color: Style.line
        }

        ColumnLayout {
            id: sheetColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: Style.fromPixel(28)
                leftMargin: Style.fromPixel(16)
                rightMargin: Style.fromPixel(16)
            }
            spacing: Style.fromPixel(14)

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Style.fromPixel(10)

                Text {
                    text: "Move to"
                    font.pixelSize: Style.fromPixel(13)
                    font.weight: Font.DemiBold
                    color: Style.inkSecondary
                }

                Flow {
                    id: moveFlow
                    Layout.fillWidth: true
                    spacing: Style.fromPixel(8)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: actionColumn.implicitHeight - 1
                radius: Style.fromPixel(18)
                color: Style.raised
                clip: true

                ColumnLayout {
                    id: actionColumn
                    width: parent.width
                    spacing: 0
                }
            }
        }
    }
}
