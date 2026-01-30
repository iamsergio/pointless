// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root

    signal okClicked

    Rectangle {
        color: "#80000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.okClicked();
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width * 0.8, Style.fromPixel(400))
            height: contentLayout.height + Style.fromPixel(48)
            color: "#131c27"
            border.width: Style.fromPixel(1)
            border.color: "#222b38"
            radius: Style.fromPixel(12)

            ColumnLayout {
                id: contentLayout
                anchors.centerIn: parent
                width: parent.width - Style.fromPixel(32)
                spacing: Style.fromPixel(24)

                Text {
                    text: "Error"
                    font.pixelSize: Style.fromPixel(24)
                    font.bold: true
                    color: "#ff4444"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: GuiController.errorController.errorText
                    font.pixelSize: Style.fromPixel(12)
                    color: "white"
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                Button {
                    id: okButton
                    text: "OK"
                    font.pixelSize: Style.fromPixel(18)
                    background: Rectangle {
                        color: "#1884f7"
                        radius: Style.fromPixel(8)
                    }
                    contentItem: Text {
                        color: "white"
                        text: "OK"
                        font.pixelSize: Style.fromPixel(18)
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Layout.fillWidth: true
                    implicitHeight: Style.fromPixel(48)
                    onClicked: {
                        root.okClicked();
                    }
                }
            }
        }
    }
}
