// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

ColumnLayout {
    id: root

    readonly property int count: taskList.count
    spacing: 0

    RowLayout {
        Layout.topMargin: Style.fromPixel(4)
        Layout.bottomMargin: Style.fromPixel(14)
        spacing: Style.fromPixel(10)

        Text {
            Layout.alignment: Qt.AlignBaseline
            text: "Later"
            font.pixelSize: Style.fromPixel(30)
            font.bold: true
            color: Style.ink
        }

        Text {
            Layout.alignment: Qt.AlignBaseline
            text: root.count === 0 ? "No tasks" : (root.count === 1 ? "1 task" : root.count + " tasks")
            font.pixelSize: Style.fromPixel(13.5)
            color: Style.inkSecondary
        }
    }

    TaskList {
        id: taskList
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: contentHeight
        Layout.maximumHeight: contentHeight
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
