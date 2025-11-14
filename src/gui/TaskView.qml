// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

ListView {
    id: root
    model: Controller.taskFilterModel

    section.property: "tagName"
    section.criteria: ViewSection.FullString
    section.delegate: Rectangle {
        width: ListView.view.width
        height: 30
        color: Style.sectionBackgroundColor

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: "foo"
            font.bold: true
            font.pixelSize: Style.sectionFontSize
            color: Style.sectionTextColor
        }
    }

    delegate: Task {
        required property string uuid
        required property string title
        required property bool isDone
        required property bool isImportant

        width: ListView.view.width
        taskUuid: uuid
        taskTitle: title
        taskIsDone: isDone
        taskIsImportant: isImportant
    }
}
