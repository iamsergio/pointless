// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "tasksByTag"
    rightButtonIcon: "\uf055"

    onRightButtonClicked: {
        GuiController.setTaskBeingEdited("", new Date(NaN), GuiController.currentTag);
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.topMargin: Style.fromPixel(4)
            Layout.bottomMargin: Style.fromPixel(14)
            spacing: Style.fromPixel(10)

            Rectangle {
                Layout.alignment: Qt.AlignVCenter
                implicitWidth: Style.fromPixel(14)
                implicitHeight: Style.fromPixel(14)
                radius: width / 2
                color: GuiController.colorFromTag(GuiController.currentTag)
            }

            Text {
                Layout.alignment: Qt.AlignBaseline
                text: GuiController.currentTag
                font.pixelSize: Style.fromPixel(30)
                font.bold: true
                color: Style.ink
            }

            Text {
                Layout.alignment: Qt.AlignBaseline
                text: taskList.count === 0 ? "No tasks" : (taskList.count === 1 ? "1 task" : taskList.count + " tasks")
                font.pixelSize: Style.fromPixel(13.5)
                color: Style.inkSecondary
            }
        }

        TaskList {
            id: taskList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: contentHeight
            Layout.maximumHeight: contentHeight
            showTags: false
            showsTagsInSecondLine: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
