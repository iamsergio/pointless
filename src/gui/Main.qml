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

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            WeekNavigator {
                Layout.fillWidth: true
            }

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
