// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageTitle: "Calendars"

    ListView {
        id: listView
        anchors {
            fill: parent
            bottomMargin: Style.fromPixel(60)
        }
        model: GuiController.calendarsModel
        spacing: Style.fromPixel(2)

        delegate: Item {
            id: delegateRoot
            required property string calendarId
            required property string title
            required property string color
            required property bool enabled

            width: listView.width
            implicitHeight: Style.fromPixel(44)

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Style.fromPixel(8)
                    rightMargin: Style.fromPixel(8)
                }
                spacing: Style.fromPixel(8)

                Rectangle {
                    radius: width / 2
                    color: delegateRoot.color
                }

                Text {
                    Layout.fillWidth: true
                    text: delegateRoot.title
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                    elide: Text.ElideRight
                }

                CheckBox {
                    checked: delegateRoot.enabled
                    onToggled: {
                        GuiController.calendarsModel.setEnabled(delegateRoot.calendarId, checked);
                    }
                }
            }
        }
    }

    Rectangle {
        id: fetchButton
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Style.fromPixel(8)
        }
        height: Style.fromPixel(44)
        color: Style.buttonColor
        radius: Style.fromPixel(8)

        Text {
            anchors.centerIn: parent
            text: "Fetch Calendar Events"
            color: Style.buttonTextColor
            font.pixelSize: Style.fromPixel(16)
        }

        MouseArea {
            anchors.fill: parent
            onClicked: GuiController.fetchCalendarEvents()
        }
    }
}
