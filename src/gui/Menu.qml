// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    border.width: 1
    border.color: "#1a1b1e"
    color: Style.taskBackground
    radius: Style.fromPixel(12)
    height: columnLayout.implicitHeight + Style.fromPixel(12)

    default property alias content: columnLayout.data

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.topMargin: Style.fromPixel(6)
        anchors.bottomMargin: Style.fromPixel(6)
        spacing: 0
    }
}
