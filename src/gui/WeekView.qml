// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Item {
    id: root

    WeekdayModel {
        id: weekdayModel
        mondayDate: Controller.navigatorStartDate
    }

    ListView {
        id: weekdayListView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        spacing: Style.fromPixel(10)
        model: weekdayModel
        delegate: ColumnLayout {
            id: column
            width: parent.width
            required property string prettyDate
            RowLayout {
                spacing: 0
                Layout.fillWidth: true
                Text {
                    font.pixelSize: Style.weekdayFontSize
                    font.bold: true
                    color: Style.weekdayTextColor
                    text: column.prettyDate
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                }
                FontAwesomeButton {
                    fontAwesomeIcon: "\uf067" // plus icon
                    iconSize: 18
                    iconColor: Style.weekdayTextColor
                    implicitWidth: 32
                    implicitHeight: 32
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    backgroundColor: "transparent"
                    onClicked: {}
                }
            }
        }
    }
}
