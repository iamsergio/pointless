// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick

import pointless 1.0

Page {
    id: root
    pageId: "tags"
    pageTitle: "Tags"
    rightButtonIcon: "\uf067"
    rightButtonObjectName: "addTagButton"
    onRightButtonClicked: addTagPopup.open()

    TagsView {
        anchors.fill: parent
    }

    overlayData: [
        AddTagPopup {
            id: addTagPopup
            anchors.fill: parent
        },
        RenameTagPopup {
            anchors.fill: parent
        }
    ]
}
