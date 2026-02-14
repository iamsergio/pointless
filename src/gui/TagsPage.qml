// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick

import pointless 1.0

Page {
    id: root
    pageId: "tags"
    pageTitle: "Tags"
    rightButtonIcon: "\uf067"
    onRightButtonClicked: addTagPopup.open()

    TagsView {
        anchors.fill: parent
    }

    AddTagPopup {
        id: addTagPopup
        anchors.fill: parent
    }
}
