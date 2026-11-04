// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import pointless 1.0

Rectangle {
    id: root
    color: Style.surface
    radius: Style.fromPixel(18)
    border.width: 1
    border.color: Style.line
    implicitHeight: calendarColumn.implicitHeight + Style.fromPixel(28)

    required property date currentDate

    signal dateSelected(date selectedDate)

    readonly property real cellSize: Style.fromPixel(33)

    function setInitialMonth() {
        if (isNaN(currentDate.getTime())) {
            calendarModel.month = new Date();
        } else {
            calendarModel.month = new Date(currentDate.getFullYear(), currentDate.getMonth(), 1);
        }
    }

    function shiftMonth(delta) {
        var d = calendarModel.month;
        calendarModel.month = new Date(d.getFullYear(), d.getMonth() + delta, 1);
    }

    CalendarModel {
        id: calendarModel
    }

    ColumnLayout {
        id: calendarColumn
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Style.fromPixel(14)
        }
        spacing: Style.fromPixel(8)

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(4)

            Text {
                text: Qt.formatDate(calendarModel.month, "MMMM yyyy")
                color: Style.ink
                font.pixelSize: Style.fromPixel(16)
                font.weight: Font.DemiBold
                Layout.fillWidth: true
            }

            IconButton {
                id: previousMonthButton
                diameter: Style.fromPixel(32)
                fontAwesomeIcon: "\uf053"
                iconSize: Style.fromPixel(13)
                onClicked: root.shiftMonth(-1)
            }

            IconButton {
                id: nextMonthButton
                diameter: Style.fromPixel(32)
                fontAwesomeIcon: "\uf054"
                iconSize: Style.fromPixel(13)
                onClicked: root.shiftMonth(1)
            }
        }

        Row {
            Layout.fillWidth: true
            Repeater {
                model: ["M", "T", "W", "T", "F", "S", "S"]
                Text {
                    required property string modelData
                    text: modelData
                    color: Style.inkTertiary
                    font.pixelSize: Style.fromPixel(11)
                    font.weight: Font.DemiBold
                    width: calendarColumn.width / 7
                    height: Style.fromPixel(20)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        GridView {
            id: daysGrid
            Layout.fillWidth: true
            Layout.preferredHeight: cellHeight * 6
            model: calendarModel
            cellWidth: width / 7
            cellHeight: root.cellSize + Style.fromPixel(4)
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
                readonly property bool isWeekend: date.getDay() === 0 || date.getDay() === 6

                Rectangle {
                    anchors.centerIn: parent
                    width: root.cellSize
                    height: width
                    radius: width / 2
                    color: delegateRoot.isSelectedDay ? Style.accent : "transparent"
                    border.width: delegateRoot.isToday && !delegateRoot.isSelectedDay ? 1 : 0
                    border.color: Style.accent

                    Text {
                        anchors.centerIn: parent
                        text: delegateRoot.day
                        color: delegateRoot.isSelectedDay ? Style.accentInk : (!delegateRoot.isCurrentMonth ? Style.inkTertiary : (delegateRoot.isWeekend ? Style.inkSecondary : Style.ink))
                        opacity: delegateRoot.isCurrentMonth || delegateRoot.isSelectedDay ? 1 : 0.5
                        font.pixelSize: Style.fromPixel(14)
                        font.weight: delegateRoot.isSelectedDay ? Font.DemiBold : Font.Normal
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
