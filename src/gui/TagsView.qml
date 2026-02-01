// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import pointless 1.0

ListView {
    id: root
    model: GuiController.tagFilterModel
    spacing: Style.fromPixel(4)
    clip: true

    delegate: Rectangle {
        id: delegateRoot
        required property string name
        required property int taskCount
        width: root.width
        height: Style.fromPixel(40)
        color: "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Style.fromPixel(4)
            anchors.rightMargin: Style.fromPixel(4)
            spacing: Style.fromPixel(8)

            Tag {
                tagName: delegateRoot.name
            }

            Text {
                text: delegateRoot.taskCount === 1 ? "1 task" : delegateRoot.taskCount + " tasks"
                color: Style.taskSecondaryTextColor
                font.pixelSize: Style.fromPixel(12)
            }

            Item {
                Layout.fillWidth: true
            }

            FontAwesomeButton {
                fontAwesomeIcon: "\uf2ed"
                iconSize: Style.fromPixel(14)
                iconColor: Style.iconColor
                backgroundColor: "transparent"
                enabled: delegateRoot.taskCount === 0
                opacity: enabled ? 1.0 : 0.3
                onClicked: GuiController.removeTag(delegateRoot.name)
            }
        }
    }
}
