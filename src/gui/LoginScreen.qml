// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

FocusScope {
    id: root

    property bool passwordVisible: false

    Rectangle {
        color: "#131c27"
        border.width: Style.fromPixel(1)
        border.color: "#222b38"
        anchors.fill: parent

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
                            text: GuiController.defaultLoginUsername
                            font.pixelSize: Style.fromPixel(16)
                            color: "white"
                            placeholderTextColor: "#8a97a8"
                            background: null
                            focus: true
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
                            echoMode: root.passwordVisible ? TextInput.Normal : TextInput.Password
                            font.pixelSize: Style.fromPixel(16)
                            color: "white"
                            placeholderTextColor: "#8a97a8"
                            background: null
                        }

                        Text {
                            text: root.passwordVisible ? "\uf06e" : "\uf070"
                            font.family: "Font Awesome 7 Free"
                            font.pixelSize: Style.fromPixel(16)
                            color: "#8a97a8"

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.passwordVisible = !root.passwordVisible
                            }
                        }
                    }
                }
            }

            Button {
                id: loginButton
                text: "Login"
                font.pixelSize: Style.fromPixel(20)
                enabled: !GuiController.isLoggingIn
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
                onClicked: {
                    GuiController.login(emailInput.text, passwordInput.text);
                }
            }

            FontAwesomeButton {
                fontAwesomeIcon: "\uf021"
                iconSize: Style.fromPixel(24)
                iconColor: "#1884f7"
                backgroundColor: "transparent"
                visible: GuiController.isLoggingIn
                enabled: false
                Layout.alignment: Qt.AlignHCenter
                rotation: GuiController.isLoggingIn ? 360 : 0
                RotationAnimation on rotation {
                    from: 0
                    to: 360
                    duration: 1000
                    loops: Animation.Infinite
                    running: GuiController.isLoggingIn
                }
                Behavior on rotation {
                    enabled: !GuiController.isLoggingIn
                    NumberAnimation {
                        duration: 200
                    }
                }
            }

            Text {
                text: GuiController.errorController.loginError
                color: "#ff4444"
                font.pixelSize: Style.fromPixel(14)
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                visible: GuiController.errorController.loginError.length > 0
                wrapMode: Text.WordWrap
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
            Text {
                text: "Offline mode"
                color: "#8a97a8"
                font.pixelSize: Style.fromPixel(16)
                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                    }
                }
            }
        }
    }
}
