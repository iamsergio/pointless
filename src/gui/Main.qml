// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "My QML Application"

    Rectangle {
        anchors.fill: parent
        color: Style.background

        LoginScreen {
            id: loginScreen
            anchors.fill: parent
            visible: true
            z: 200
        }

        // Overlaid refresh button
        FontAwesomeButton {
            fontAwesomeIcon: "\uf021" // FontAwesome refresh icon
            iconSize: 24
            iconColor: "white"
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 16
            anchors.rightMargin: 16
            z: 100
            onClicked: Controller.refresh()
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                ViewButton {
                    text: "Week"
                    isActive: Controller.currentViewType === Controller.Week
                    onClicked: Controller.currentViewType = Controller.Week
                }

                ViewButton {
                    text: "Soon"
                    isActive: Controller.currentViewType === Controller.Soon
                    onClicked: Controller.currentViewType = Controller.Soon
                }

                ViewButton {
                    text: "Later"
                    isActive: Controller.currentViewType === Controller.Later
                    onClicked: Controller.currentViewType = Controller.Later
                }
            }

            WeekNavigator {
                Layout.fillWidth: true
                visible: Controller.currentViewType === Controller.Week
            }

            WeekView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: Controller.currentViewType === Controller.Week
            }

            SoonView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: Controller.currentViewType === Controller.Soon
            }

            LaterView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: Controller.currentViewType === Controller.Later
            }
        }
    }
}
