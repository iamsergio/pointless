// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

ColumnLayout {
    id: root

    readonly property int count: taskList.count
    spacing: 0

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    RowLayout {
        Layout.topMargin: Style.fromPixel(4)
        Layout.bottomMargin: Style.fromPixel(14)
        spacing: Style.fromPixel(10)

        Text {
            Layout.alignment: Qt.AlignBaseline
            text: "Goals"
            font.pixelSize: Style.fromPixel(30)
            font.bold: true
            color: Style.ink
        }

        Text {
            Layout.alignment: Qt.AlignBaseline
            visible: root.count > 0
            text: root.count === 1 ? "1 goal" : root.count + " goals"
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

    ColumnLayout {
        visible: root.count === 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: Style.fromPixel(18)
        Layout.rightMargin: Style.fromPixel(18)
        Layout.bottomMargin: Style.fromPixel(150)
        spacing: Style.fromPixel(12)

        Item {
            Layout.fillHeight: true
        }

        Rectangle {
            implicitWidth: Style.fromPixel(52)
            implicitHeight: Style.fromPixel(52)
            radius: Style.fromPixel(18)
            color: Style.accentSoft

            Text {
                anchors.centerIn: parent
                text: "\uf024"
                font.family: fontAwesome.name
                font.pixelSize: Style.fromPixel(24)
                color: Style.accentText
            }
        }

        Text {
            text: "No goals yet"
            font.pixelSize: Style.fromPixel(22)
            font.weight: Font.DemiBold
            color: Style.ink
        }

        Text {
            Layout.fillWidth: true
            text: "A goal is a task you work towards over weeks or months. Turn on Goal when you edit a task."
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: Style.fromPixel(14.5)
            color: Style.inkSecondary
        }

        Item {
            Layout.fillHeight: true
        }
    }

    Item {
        visible: root.count > 0
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
