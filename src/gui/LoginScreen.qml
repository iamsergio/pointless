// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#131c27"
    radius: 16
    border.width: Style.fromPixel(1)
    border.color: "#222b38"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Style.fromPixel(32)
        width: parent.width * 0.85

        Text {
            text: "Pointless"
            font.pixelSize: Style.fromPixel(32)
            font.bold: true
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        ColumnLayout {
            spacing: 16
            Layout.fillWidth: true

            Text {
                text: "Email"
                font.pixelSize: Style.fromPixel(16)
                color: "white"
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(48)
                color: "#192233"
                radius: 8

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Style.fromPixel(12)
                    anchors.rightMargin: Style.fromPixel(12)
                    spacing: Style.fromPixel(8)

                    Text {
                        text: "\uf0e0"
                        font.family: "Font Awesome 7 Free"
                        font.pixelSize: Style.fromPixel(16)
                        color: "#8a97a8"
                    }

                    TextField {
                        id: emailInput
                        Layout.fillWidth: true
                        placeholderText: "Enter your email"
                        font.pixelSize: Style.fromPixel(16)
                        color: "white"
                        placeholderTextColor: "#8a97a8"
                        background: null
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    text: "Password"
                    font.pixelSize: Style.fromPixel(16)
                    color: "white"
                    Layout.alignment: Qt.AlignLeft
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "Forgot Password?"
                    color: "#1884f7"
                    font.pixelSize: Style.fromPixel(14)
                    Layout.alignment: Qt.AlignRight
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(48)
                color: "#192233"
                radius: 8

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    spacing: 8

                    Text {
                        text: "\uf023"
                        font.family: "Font Awesome 7 Free"
                        font.pixelSize: Style.fromPixel(16)
                        color: "#8a97a8"
                    }

                    TextField {
                        id: passwordInput
                        Layout.fillWidth: true
                        placeholderText: "Enter your password"
                        echoMode: TextInput.Password
                        font.pixelSize: Style.fromPixel(16)
                        color: "white"
                        placeholderTextColor: "#8a97a8"
                        background: null
                    }

                    Text {
                        text: "\uf070"
                        font.family: "Font Awesome 7 Free"
                        font.pixelSize: Style.fromPixel(16)
                        color: "#8a97a8"
                    }
                }
            }
        }

        Button {
            id: loginButton
            text: "Login"
            font.pixelSize: Style.fromPixel(20)
            background: Rectangle {
                color: "#1884f7"
                radius: 12
            }
            contentItem: Text {
                color: "white"
                text: "Login"
                font.pixelSize: Style.fromPixel(20)
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Layout.fillWidth: true
            implicitHeight: Style.fromPixel(56)
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 4
            Text {
                text: "Don't have an account?"
                color: "#8a97a8"
                font.pixelSize: Style.fromPixel(16)
            }
            Text {
                text: "Sign Up"
                color: "#1884f7"
                font.pixelSize: Style.fromPixel(16)
            }
        }
    }
}
