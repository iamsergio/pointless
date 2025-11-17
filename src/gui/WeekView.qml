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
            required property string prettyDate
            Text {
                font.pixelSize: Style.weekdayFontSize
                color: Style.weekdayTextColor
                text: parent.prettyDate
            }
        }
    }
}
