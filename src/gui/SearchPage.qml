// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Fusion as QC
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "search"

    onVisibleChanged: {
        if (visible)
            searchInput.forceActiveFocus();
    }

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    headerData: Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: Style.fromPixel(42)
        radius: 21
        color: Style.surface
        border.width: searchInput.activeFocus ? 1.5 : 1
        border.color: searchInput.activeFocus ? Style.accent : Style.line

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Style.fromPixel(14)
            anchors.rightMargin: Style.fromPixel(6)
            spacing: Style.fromPixel(8)

            Text {
                text: "\uf002"
                font.family: fontAwesome.name
                font.pixelSize: Style.fromPixel(14)
                color: Style.inkTertiary
            }

            QC.TextField {
                id: searchInput
                objectName: "searchInput"
                Layout.fillWidth: true
                placeholderText: "Search tasks..."
                color: Style.ink
                placeholderTextColor: Style.inkTertiary
                background: null
                font.pixelSize: Style.fromPixel(15)
                selectByMouse: true
                verticalAlignment: Text.AlignVCenter
                focus: root.visible
            }

            IconButton {
                visible: searchInput.text.length > 0
                diameter: Style.fromPixel(30)
                fontAwesomeIcon: "\uf00d"
                iconSize: Style.fromPixel(14)
                iconColor: Style.inkTertiary
                onClicked: searchInput.clear()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.fromPixel(12)

        Text {
            visible: searchInput.text.length > 0
            text: taskList.count === 1 ? "1 task" : taskList.count + " tasks"
            font.pixelSize: Style.fromPixel(13.5)
            color: Style.inkTertiary
        }

        TaskList {
            id: taskList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: contentHeight
            Layout.maximumHeight: contentHeight
            searchText: searchInput.text
            showsTagsInSecondLine: true
            showsDate: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
