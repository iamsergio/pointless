// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    color: Style.calendarBackground
    radius: Style.fromPixel(10)

    CalendarModel {
        id: calendarModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(16)
        spacing: Style.fromPixel(10)

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(10)

            Text {
                text: "\uf073" // Calendar icon
                font.family: "Font Awesome 6 Free"
                font.pixelSize: Style.fromPixel(16)
                color: Style.calendarSecondaryText
            }

            Text {
                text: "Today, " + Qt.formatDate(new Date(), "MMM d")
                color: Style.calendarText
                font.pixelSize: Style.fromPixel(16)
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            FontAwesomeButton {
                id: nextMonthButton
                fontAwesomeIcon: "\uf054" // Chevron right
                iconSize: Style.fromPixel(14)
                iconColor: Style.calendarSecondaryText
                backgroundColor: "transparent"
                onClicked: {
                    var d = calendarModel.month
                    var newDate = new Date(d.getFullYear(), d.getMonth() + 1, 1)
                    calendarModel.month = newDate
                }
            }
        }

        // Weekday headers
        RowLayout {
            Layout.fillWidth: true
            Repeater {
                model: ["M", "T", "W", "T", "F", "S", "S"]
                Text {
                    required property string modelData
                    text: modelData
                    color: Style.calendarSecondaryText
                    font.pixelSize: Style.fromPixel(12)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        // Days Grid
        GridView {
            id: daysGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: calendarModel
            cellWidth: width / 7
            cellHeight: height / 6
            interactive: false

            delegate: Item {
                id: delegateRoot
                width: GridView.view.cellWidth
                height: GridView.view.cellHeight

                required property bool isToday
                required property string day
                required property bool isCurrentMonth

                Rectangle {
                    anchors.centerIn: parent
                    width: Math.min(parent.width, parent.height) * 0.8
                    height: width
                    radius: width / 2
                    color: delegateRoot.isToday ? Style.calendarHighlight : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: delegateRoot.day
                        color: delegateRoot.isToday ? "white" : (delegateRoot.isCurrentMonth ? Style.calendarText : "transparent")
                        font.pixelSize: Style.fromPixel(14)
                    }
                }
            }
        }
    }
}
