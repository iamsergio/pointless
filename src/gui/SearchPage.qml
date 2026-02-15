// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "search"
    pageTitle: "Search"

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.fromPixel(12)

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Style.fromPixel(44)
            color: Style.taskBackground
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.margins: Style.fromPixel(10)
                spacing: Style.fromPixel(8)

                Text {
                    text: "\uf002"
                    font.family: fontAwesome.name
                    font.pixelSize: Style.fromPixel(14)
                    color: "#888888"
                }

                QC.TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    placeholderText: "Search tasks..."
                    color: "white"
                    placeholderTextColor: "#888888"
                    background: null
                    font.pixelSize: Style.fromPixel(14)
                    selectByMouse: true
                    verticalAlignment: Text.AlignVCenter
                    focus: root.visible
                }
            }
        }

        TaskView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            searchText: searchInput.text
            showsTagsInSecondLine: true
            showsDate: true
        }
    }
}
