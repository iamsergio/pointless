// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts 1.15
import pointless 1.0

FocusScope {
    id: root

    property bool passwordVisible: false

    LoginScreenController {
        id: controller
    }

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    Rectangle {
        color: Style.background
        anchors.fill: parent

        ColumnLayout {
            anchors {
                fill: parent
                margins: Style.fromPixel(22)
            }
            spacing: 0

            Rectangle {
                Layout.preferredWidth: Style.fromPixel(56)
                Layout.preferredHeight: Style.fromPixel(56)
                radius: width / 2
                color: Style.accent

                Text {
                    anchors.centerIn: parent
                    text: "\uf00c"
                    font.family: fontAwesome.name
                    font.pixelSize: Style.fromPixel(23)
                    color: Style.accentInk
                }
            }

            Text {
                Layout.topMargin: Style.fromPixel(16)
                text: "Pointless"
                font.pixelSize: Style.fromPixel(30)
                font.weight: Font.Bold
                color: Style.ink
            }

            Text {
                Layout.topMargin: Style.fromPixel(6)
                Layout.fillWidth: true
                text: "Log in to sync your tasks between devices."
                font.pixelSize: Style.fromPixel(15)
                color: Style.inkSecondary
                wrapMode: Text.WordWrap
            }

            Text {
                Layout.topMargin: Style.fromPixel(28)
                text: "Email"
                font.pixelSize: Style.fromPixel(13)
                font.weight: Font.DemiBold
                color: Style.inkSecondary
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: Style.fromPixel(8)
                implicitHeight: Style.fromPixel(50)
                color: Style.raised
                radius: Style.fromPixel(14)
                border.width: emailInput.activeFocus ? 1.5 : 0
                border.color: Style.accent

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Style.fromPixel(16)
                    anchors.rightMargin: Style.fromPixel(16)
                    spacing: Style.fromPixel(12)

                    Text {
                        text: "\uf0e0"
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(15)
                        color: Style.inkTertiary
                    }

                    TextField {
                        id: emailInput
                        Layout.fillWidth: true
                        placeholderText: "Enter your email"
                        text: controller.username
                        onTextEdited: controller.username = text
                        font.pixelSize: Style.fromPixel(16)
                        color: Style.ink
                        placeholderTextColor: Style.inkTertiary
                        leftPadding: 0
                        background: null
                        focus: true
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Style.fromPixel(18)
                spacing: 0

                Text {
                    text: "Password"
                    font.pixelSize: Style.fromPixel(13)
                    font.weight: Font.DemiBold
                    color: Style.inkSecondary
                }

                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: "Forgot password?"
                    color: Style.accentText
                    font.pixelSize: Style.fromPixel(13)
                    font.weight: Font.DemiBold
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: Style.fromPixel(8)
                implicitHeight: Style.fromPixel(50)
                color: Style.raised
                radius: Style.fromPixel(14)
                border.width: passwordInput.activeFocus ? 1.5 : 0
                border.color: Style.accent

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Style.fromPixel(16)
                    anchors.rightMargin: Style.fromPixel(6)
                    spacing: Style.fromPixel(12)

                    Text {
                        text: "\uf023"
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(15)
                        color: Style.inkTertiary
                    }

                    TextField {
                        id: passwordInput
                        Layout.fillWidth: true
                        placeholderText: "Enter your password"
                        text: controller.password
                        onTextEdited: controller.password = text
                        echoMode: root.passwordVisible ? TextInput.Normal : TextInput.Password
                        font.pixelSize: Style.fromPixel(16)
                        color: Style.ink
                        placeholderTextColor: Style.inkTertiary
                        leftPadding: 0
                        background: null
                    }

                    IconButton {
                        diameter: Style.fromPixel(38)
                        iconSize: Style.fromPixel(15)
                        iconColor: Style.inkTertiary
                        fontAwesomeIcon: root.passwordVisible ? "\uf070" : "\uf06e"
                        onClicked: root.passwordVisible = !root.passwordVisible
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Style.fromPixel(22)
                spacing: Style.fromPixel(8)

                Button {
                    id: loginButton
                    text: "Log in"
                    enabled: !GuiController.isLoggingIn
                    Layout.fillWidth: true
                    implicitHeight: Style.fromPixel(50)
                    background: Rectangle {
                        color: Style.accent
                        radius: Style.fromPixel(15)
                        opacity: !loginButton.enabled ? 0.5 : (loginButton.pressed ? 0.8 : 1)
                    }
                    contentItem: Text {
                        color: Style.accentInk
                        text: "Log in"
                        font.pixelSize: Style.fromPixel(15.5)
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        GuiController.login(controller.username, controller.password);
                    }
                }

                IconButton {
                    fontAwesomeIcon: "\uf084"
                    iconSize: Style.fromPixel(18)
                    fillColor: Style.raised
                    visible: !GuiController.isMobile
                    diameter: Style.fromPixel(50)
                    onClicked: {
                        GuiController.fetchPassStoreCredentials();
                    }
                }
            }

            IconButton {
                fontAwesomeIcon: "\uf021"
                iconSize: Style.fromPixel(22)
                iconColor: Style.accentText
                visible: GuiController.isLoggingIn
                enabled: false
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Style.fromPixel(10)
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
                color: Style.danger
                font.pixelSize: Style.fromPixel(14)
                Layout.fillWidth: true
                Layout.topMargin: Style.fromPixel(12)
                visible: GuiController.errorController.loginError.length > 0
                wrapMode: Text.WordWrap
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Style.fromPixel(18)
                textFormat: Text.StyledText
                text: "No account yet? <font color=\"" + Style.accentText + "\"><b>Sign up</b></font>"
                color: Style.inkSecondary
                font.pixelSize: Style.fromPixel(14)
            }

            Item {
                Layout.fillHeight: true
                Layout.minimumHeight: Style.fromPixel(16)
            }

            Button {
                id: offlineButton
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(50)
                onClicked: GuiController.enableOfflineMode()
                background: Rectangle {
                    color: Style.raised
                    radius: Style.fromPixel(15)
                    opacity: offlineButton.pressed ? 0.8 : 1
                }
                contentItem: Text {
                    color: Style.ink
                    text: "Continue offline"
                    font.pixelSize: Style.fromPixel(15.5)
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Style.fromPixel(8)
                text: "Your tasks stay on this device."
                color: Style.inkSecondary
                font.pixelSize: Style.fromPixel(12)
            }
        }
    }
}
