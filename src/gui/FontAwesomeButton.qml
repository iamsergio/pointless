// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import pointless 1.0

Button {
    id: root

    property string fontAwesomeIcon: "\uf00c"
    property int iconSize: Style.fromPixel(16)
    property color iconColor: "white" // the text foreground color
    property color backgroundColor: Style.buttonBackground // the background of fontawesome icon
    hoverEnabled: true

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    contentItem: Text {
        text: root.fontAwesomeIcon
        font.family: fontAwesome.name
        font.pixelSize: root.pressed ? root.iconSize + Style.fromPixel(2) : root.iconSize
        color: (root.pressed || root.hovered) ? Qt.lighter(root.iconColor) : root.iconColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        id: backgroundRect
        color: root.backgroundColor
        radius: 4
    }
}
