// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls.Fusion
import pointless 1.0

Button {
    id: root

    property string fontAwesomeIcon: ""
    property string textLabel: ""
    property int iconSize: Style.fromPixel(20)
    property color iconColor: Style.inkSecondary
    property color fillColor: "transparent"
    property int diameter: Style.fromPixel(40)

    hoverEnabled: true
    readonly property bool hasLabel: textLabel.length > 0
    implicitWidth: hasLabel ? labelText.implicitWidth + Style.fromPixel(hasLabel && isFilled ? 32 : 20) : diameter
    implicitHeight: diameter

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    contentItem: Text {
        id: labelText
        text: root.hasLabel ? root.textLabel : root.fontAwesomeIcon
        font.family: root.hasLabel ? Application.font.family : fontAwesome.name
        font.weight: root.hasLabel ? Font.DemiBold : Font.Normal
        font.pixelSize: root.iconSize
        color: root.iconColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    readonly property bool isFilled: fillColor.a > 0

    background: Rectangle {
        radius: width / 2
        color: root.isFilled && (root.pressed || root.hovered) ? Qt.lighter(root.fillColor, root.pressed ? 1.2 : 1.1) : root.fillColor
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Style.raised
            visible: !root.isFilled
            opacity: root.pressed ? 1 : (root.hovered ? 0.6 : 0)
        }
    }
}
