// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    border.width: 1
    border.color: "black"
    color: "white"
    height: columnLayout.implicitHeight

    default property alias content: columnLayout.data

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
    }
}
