// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    color: Style.calendarBackground
    radius: Style.fromPixel(10)

    required property date currentDate

    signal dateSelected(date selectedDate)

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
                font.pixelSize: Style.fromPixel(20)
                color: Style.calendarSecondaryText
            }

            Text {
                text: Qt.formatDate(calendarModel.month, "MMMM yyyy")
                color: Style.calendarText
                font.pixelSize: Style.fromPixel(16)
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            FontAwesomeButton {
                id: previousMonthButton
                fontAwesomeIcon: "\uf053" // Chevron left
                iconSize: Style.fromPixel(18)
                iconColor: Style.calendarSecondaryText
                backgroundColor: "transparent"
                onClicked: {
                    var d = calendarModel.month;
                    var newDate = new Date(d.getFullYear(), d.getMonth() - 1, 1);
                    calendarModel.month = newDate;
                }
            }

            FontAwesomeButton {
                id: nextMonthButton
                fontAwesomeIcon: "\uf054" // Chevron right
                iconSize: previousMonthButton.iconSize
                iconColor: Style.calendarSecondaryText
                backgroundColor: "transparent"
                onClicked: {
                    var d = calendarModel.month;
                    var newDate = new Date(d.getFullYear(), d.getMonth() + 1, 1);
                    calendarModel.month = newDate;
                }
            }
        }

        // Weekday headers
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: Style.fromPixel(20)
            Row {
                anchors.fill: parent
                anchors.topMargin: Style.fromPixel(5)
                Repeater {
                    model: ["M", "T", "W", "T", "F", "S", "S"]
                    Text {
                        required property string modelData
                        text: modelData
                        color: Style.calendarSecondaryText
                        font.pixelSize: Style.fromPixel(16)
                        width: parent.width / 7
                        horizontalAlignment: Text.AlignHCenter
                    }
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

                required property date date
                required property bool isToday
                required property string day
                required property bool isCurrentMonth
                readonly property bool isSelectedDay: date.getDate() === root.currentDate.getDate() && date.getMonth() === root.currentDate.getMonth() && date.getFullYear() === root.currentDate.getFullYear()

                Rectangle {
                    anchors.centerIn: parent
                    width: Math.min(parent.width, parent.height) * 0.8
                    height: width
                    radius: width / 2
                    color: delegateRoot.isSelectedDay ? Style.calendarHighlight : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: delegateRoot.day
                        color: delegateRoot.isSelectedDay ? "white" : (delegateRoot.isToday ? Style.calendarHighlight : (delegateRoot.isCurrentMonth ? Style.calendarText : Style.calendarDisabledText))
                        font.pixelSize: Style.fromPixel(14)
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.dateSelected(delegateRoot.date);
                        }
                    }
                }
            }
        }
    }
}
