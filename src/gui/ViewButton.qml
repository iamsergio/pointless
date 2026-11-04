// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import pointless 1.0

Button {
    id: root

    property bool isActive: false
    property string fontAwesomeIcon: ""
    // So tests read the same property as in the Slint UI
    checked: isActive

    implicitWidth: Style.fromPixel(80)
    implicitHeight: Style.fromPixel(50)

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    contentItem: ColumnLayout {
        spacing: Style.fromPixel(2)

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.fontAwesomeIcon
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(18)
            color: root.isActive ? Style.accentText : Style.inkTertiary
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.text
            font.pixelSize: Style.fromPixel(11)
            font.weight: Font.DemiBold
            color: root.isActive ? Style.accentText : Style.inkTertiary
        }
    }

    background: Rectangle {
        visible: root.isActive
        color: Style.accentSoft
        radius: Style.fromPixel(25)
    }
}
