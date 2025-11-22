// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    height: Style.fromPixel(50)
    color: Style.taskBackground
    radius: 4

    signal textEntered(string text)

    RowLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(10)
        spacing: Style.fromPixel(10)

        // Blue square icon
        Rectangle {
            width: Style.fromPixel(16)
            height: Style.fromPixel(16)
            color: Style.calendarHighlight
            radius: 2
        }

        TextField {
            id: input
            Layout.fillWidth: true
            placeholderText: "New Task Title"
            color: "white"
            background: null
            font.pixelSize: Style.fromPixel(14)
            selectByMouse: true
            verticalAlignment: Text.AlignVCenter

            onAccepted: {
                if (text.trim() !== "") {
                    root.textEntered(text)
                    text = ""
                }
            }
        }
    }
}
