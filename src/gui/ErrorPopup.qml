// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts 1.15
import pointless 1.0

Item {
    id: root

    signal okClicked

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    Rectangle {
        color: "#8C000000"
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.okClicked();
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

                Rectangle {
                    implicitWidth: Style.fromPixel(44)
                    implicitHeight: Style.fromPixel(44)
                    radius: Style.fromPixel(14)
                    color: Style.dangerSoft

                    Text {
                        anchors.centerIn: parent
                        text: ""
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(20)
                        color: Style.danger
                    }
                }

                Text {
                    text: "Something went wrong"
                    font.pixelSize: Style.fromPixel(21)
                    font.weight: Font.DemiBold
                    color: Style.ink
                    Layout.fillWidth: true
                }

                Text {
                    text: GuiController.errorController.errorText
                    font.pixelSize: Style.fromPixel(14)
                    color: Style.inkSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Button {
                    id: okButton
                    text: "OK"
                    Layout.fillWidth: true
                    Layout.topMargin: Style.fromPixel(4)
                    implicitHeight: Style.fromPixel(48)
                    background: Rectangle {
                        color: Style.accent
                        radius: Style.fromPixel(15)
                        opacity: okButton.pressed ? 0.8 : 1
                    }
                    contentItem: Text {
                        color: Style.accentInk
                        text: "OK"
                        font.pixelSize: Style.fromPixel(15.5)
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        root.okClicked();
                    }
                }
            }
        }
    }
}
