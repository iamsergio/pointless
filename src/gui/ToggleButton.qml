// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

Item {
    id: root

    property bool toggled: false

    implicitWidth: Style.fromPixel(50)
    implicitHeight: Style.fromPixel(28)

    Rectangle {
        id: background
        anchors.fill: parent
        radius: height / 2
        color: root.toggled ? Style.buttonColor : "#6b7280"

        Behavior on color {
            ColorAnimation {
                duration: 150
            }
        }

        Rectangle {
            id: circle
            width: parent.height - Style.fromPixel(6)
            height: width
            radius: width / 2
            color: "#ffffff"
            anchors.verticalCenter: parent.verticalCenter
            x: root.toggled ? parent.width - width - Style.fromPixel(3) : Style.fromPixel(3)

            Behavior on x {
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.toggled = !root.toggled
    }
}
