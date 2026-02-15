// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick

import pointless 1.0

Page {
    id: root
    pageId: "tasksByTag"
    pageTitle: GuiController.currentTag
    rightButtonIcon: "\uf055"

    onRightButtonClicked: {
        GuiController.setTaskBeingEdited("", new Date(NaN), GuiController.currentTag);
    }

    TaskView {
        anchors.fill: parent
        showTags: false
        showsTagsInSecondLine: true
    }
}
