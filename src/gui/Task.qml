// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

Rectangle {
    id: root

    required property string taskUuid
    required property string taskTitle
    required property bool taskIsDone
    required property bool taskIsImportant

    height: 50
    border.width: 1
    border.color: "black"
    color: "transparent"

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        text: root.taskTitle
    }
}
