// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick

import pointless 1.0

Page {
    id: root
    pageId: "tags"
    pageTitle: "Tags"

    TagsView {
        anchors.fill: parent
    }
}
