// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15

import pointless 1.0

ListView {
    id: root

    clip: true

    TaskFilterModel {
        id: taskFilterModel
        viewType: Controller.currentViewType
    }

    model: taskFilterModel

    delegate: Task {
        required property string uuid
        required property string title
        required property bool isDone
        required property bool isImportant
        required property var dueDate
        required property string tagName

        width: ListView.view.width
        taskUuid: uuid
        taskTitle: title
        taskIsDone: isDone
        taskIsImportant: isImportant
        taskDueDate: dueDate
        taskTagName: tagName
    }
}
