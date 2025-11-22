// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts
import pointless

ApplicationWindow {
    visible: true
    width: Style.fromPixel(640)
    height: Style.fromPixel(480)
    visibility: Controller.isMobile ? ApplicationWindow.FullScreen : ApplicationWindow.Windowed
    flags: Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint

    title: "Pointless"
    topPadding: 0
    bottomPadding: 0

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: parent.SafeArea.margins.top

        color: Style.background

        LoginScreen {
            id: loginScreen
            anchors.fill: parent
            visible: false
            z: 200
        }

        // Overlaid refresh button
        FontAwesomeButton {
            fontAwesomeIcon: "\uf021" // FontAwesome refresh icon
            iconSize: Style.fromPixel(16)
            iconColor: Style.iconColor
            backgroundColor: "transparent"
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 16
            anchors.rightMargin: 16
            z: 100
            onClicked: Controller.refresh()
        }

        FontAwesomeButton {
            fontAwesomeIcon: "\uf121"
            iconSize: Style.fromPixel(16)
            iconColor: "red"
            backgroundColor: "transparent"
            anchors {
                left: parent.left
                bottom: parent.bottom
                leftMargin: 16
                bottomMargin: 16
            }
            visible: Controller.isDebug
            z: 100
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Style.fromPixel(20)

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                spacing: Style.fromPixel(10)

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

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(1)
                color: Style.taskBackground
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
