// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "cleanup"
    pageTitle: "Cleanup"

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    Rectangle {
        id: card
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        implicitHeight: cardColumn.implicitHeight
        height: implicitHeight
        radius: Style.fromPixel(18)
        color: Style.surface
        clip: true

        component ActionRow: Item {
            id: actionRow
            property string icon: ""
            property string title: ""
            property string description: ""
            property string buttonText: ""
            property color tileColor: Style.raised
            property color tileInk: Style.inkSecondary
            property color buttonColor: Style.raised
            property color buttonInk: Style.ink
            property bool showDivider: true
            signal triggered

            Layout.fillWidth: true
            implicitHeight: textColumn.implicitHeight + Style.fromPixel(32)

            Rectangle {
                id: tile
                x: Style.fromPixel(16)
                y: Style.fromPixel(16)
                width: Style.fromPixel(32)
                height: width
                radius: Style.fromPixel(10)
                color: actionRow.tileColor

                Text {
                    anchors.centerIn: parent
                    text: actionRow.icon
                    font.family: fontAwesome.name
                    font.pixelSize: Style.fromPixel(14)
                    color: actionRow.tileInk
                }
            }

            ColumnLayout {
                id: textColumn
                anchors {
                    left: tile.right
                    leftMargin: Style.fromPixel(12)
                    right: parent.right
                    rightMargin: Style.fromPixel(16)
                    top: parent.top
                    topMargin: Style.fromPixel(16)
                }
                spacing: Style.fromPixel(4)

                Text {
                    Layout.fillWidth: true
                    text: actionRow.title
                    color: Style.ink
                    font.pixelSize: Style.fromPixel(15.5)
                }

                Text {
                    Layout.fillWidth: true
                    text: actionRow.description
                    color: Style.inkSecondary
                    font.pixelSize: Style.fromPixel(13)
                    wrapMode: Text.WordWrap
                }

                Button {
                    id: actionButton
                    Layout.topMargin: Style.fromPixel(6)
                    implicitHeight: Style.fromPixel(32)
                    implicitWidth: buttonLabel.implicitWidth + Style.fromPixel(32)
                    onClicked: actionRow.triggered()
                    background: Rectangle {
                        radius: height / 2
                        color: actionRow.buttonColor
                        opacity: actionButton.pressed ? 0.8 : 1
                    }
                    contentItem: Text {
                        id: buttonLabel
                        text: actionRow.buttonText
                        color: actionRow.buttonInk
                        font.pixelSize: Style.fromPixel(13.5)
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Rectangle {
                visible: actionRow.showDivider
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Style.line
            }
        }

        ColumnLayout {
            id: cardColumn
            width: parent.width
            spacing: 0

            ActionRow {
                icon: "\uf51a"
                title: "Clean up old tasks"
                description: "Removes tasks you finished more than two weeks ago. Yearly tasks are kept."
                buttonText: "Clean up"
                onTriggered: GuiController.cleanupOldData()
            }

            ActionRow {
                icon: "\uf073"
                title: "Delete calendar events"
                description: "Removes every task imported from your calendars. You can fetch them again from Calendars."
                buttonText: "Delete events"
                tileColor: Style.dangerSoft
                tileInk: Style.danger
                buttonColor: Style.dangerSoft
                buttonInk: Style.danger
                showDivider: false
                onTriggered: GuiController.deleteAllCalendarEvents()
            }
        }
    }
}
