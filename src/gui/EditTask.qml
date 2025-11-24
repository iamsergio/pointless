// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    color: Style.background

    signal backClicked()
    signal saveClicked()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(16)
        spacing: Style.fromPixel(20)

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(10)

            FontAwesomeButton {
                fontAwesomeIcon: "\uf060" // Arrow left
                backgroundColor: "transparent"
                iconColor: "white"
                onClicked: root.backClicked()
            }

            Text {
                text: "Edit Task"
                color: "white"
                font.pixelSize: Style.fromPixel(18)
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            FontAwesomeButton {
                fontAwesomeIcon: "\uf00c" // Check
                backgroundColor: "transparent"
                iconColor: "white"
                onClicked: root.saveClicked()
            }
        }

        // Title Section
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
                Layout.fillWidth: true
            }
        }

        // Tag Section
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
                implicitHeight: Style.fromPixel(50)
                color: Style.taskBackground
                radius: 4

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Style.fromPixel(10)

                    Text {
                        text: "life" // Mock tag
                        color: "white"
                        font.pixelSize: Style.fromPixel(14)
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "+ Add New"
                        color: "#808080"
                        font.pixelSize: Style.fromPixel(12)
                    }
                }
            }
        }

        // Date Section
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
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
