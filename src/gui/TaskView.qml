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
        height: Style.fromPixel(40)
        color: "transparent"

        required property string section

        Text {
            anchors.left: parent.left
            anchors.leftMargin: Style.fromPixel(10)
            anchors.verticalCenter: parent.verticalCenter
            text: parent.section
            font.bold: true
            font.pixelSize: Style.fromPixel(20)
            color: Style.sectionTextColor
        }
    }

    delegate: Task {
        required property string uuid
        required property string title
        required property bool isDone
        required property bool isImportant
        required property var dueDate

        width: ListView.view.width
        taskUuid: uuid
        taskTitle: title
        taskIsDone: isDone
        taskIsImportant: isImportant
        taskDueDate: dueDate
    }
}
