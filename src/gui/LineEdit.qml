// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

FocusScope {
    id: root

    height: Style.fromPixel(50)

    signal textEntered(string text)
    property alias text: input.text

    function clear() {
        input.text = "";
    }

    Rectangle {
        height: parent.height
        color: Style.taskBackground
        radius: 4

        RowLayout {
            anchors.fill: parent
            anchors.margins: Style.fromPixel(10)
            spacing: Style.fromPixel(10)

            TextField {
                id: input
                Layout.fillWidth: true
                placeholderText: "New Task Title"
                color: "white"
                placeholderTextColor: "#888888"
                background: null
                font.pixelSize: Style.fromPixel(14)
                selectByMouse: true
                verticalAlignment: Text.AlignVCenter
                focus: true

                onAccepted: {
                    if (text.trim() !== "") {
                        root.textEntered(text);
                        text = "";
                    }
                }
            }
        }
    }
}
