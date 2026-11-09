// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls.Fusion as QC
import QtQuick.Layouts 1.15

import pointless 1.0

Item {
    id: root

    // for Spix
    readonly property alias weekdayModelCount: weekdayModel.count
    readonly property alias weekdayFilterModelCount: weekdayFilterModel.count
    readonly property alias count: weekdayFilterModel.count

    WeekdayModel {
        id: weekdayModel
        mondayDate: GuiController.navigatorStartDate
    }

    WeekdayFilterModel {
        id: weekdayFilterModel
        source: weekdayModel
    }

    function weekdayItemAt(i: int): var {
        return weekdayListView.itemAtIndex(i);
    }

    function scrollToDate(target) {
        for (let i = 0; i < weekdayListView.count; ++i) {
            const item = weekdayItemAt(i);
            if (item && item.date.getTime() === target.getTime()) {
                weekdayListView.positionViewAtIndex(i, ListView.Beginning);
                return;
            }
        }
    }

    RowLayout {
        id: dayStrip
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: Style.fromPixel(14)
        }
        height: implicitHeight
        spacing: 0

        Repeater {
            model: weekdayModel

            delegate: Item {
                id: dayCell

                required property date date
                required property bool isToday
                required property bool isPast
                required property TaskFilterModel tasks

                readonly property bool isWeekend: date.getDay() === 0 || date.getDay() === 6

                Layout.fillWidth: true
                Layout.preferredWidth: 1
                implicitHeight: dayColumn.implicitHeight
                opacity: isPast ? 0.35 : 1

                ColumnLayout {
                    id: dayColumn
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.fromPixel(4)

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: Qt.formatDate(dayCell.date, "ddd")
                        font.pixelSize: Style.fromPixel(11.5)
                        font.weight: Font.DemiBold
                        color: dayCell.isToday ? Style.accentText : Style.inkTertiary
                    }

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        implicitWidth: Style.fromPixel(34)
                        implicitHeight: Style.fromPixel(34)
                        radius: width / 2
                        color: dayCell.isToday ? Style.accent : "transparent"

                        Text {
                            anchors.centerIn: parent
                            text: dayCell.date.getDate()
                            font.pixelSize: Style.fromPixel(18)
                            font.weight: Font.DemiBold
                            color: dayCell.isToday ? Style.accentInk : (dayCell.isWeekend ? Style.inkSecondary : Style.ink)
                        }
                    }

                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: Style.fromPixel(5)
                        spacing: Style.fromPixel(3)

                        Repeater {
                            model: Math.min(4, dayCell.tasks.count)

                            Rectangle {
                                width: Style.fromPixel(5)
                                height: Style.fromPixel(5)
                                radius: width / 2
                                color: dayCell.isWeekend ? Style.inkTertiary : Style.accent
                            }
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: !dayCell.isPast
                    onClicked: root.scrollToDate(dayCell.date)
                }
            }
        }
    }

    Item {
        id: stripSpacer
        anchors {
            left: parent.left
            right: parent.right
            top: dayStrip.bottom
        }
        height: Style.fromPixel(12)
    }

    ListView {
        id: weekdayListView

        cacheBuffer: 1000

        anchors {
            left: parent.left
            right: parent.right
            top: stripSpacer.bottom
            bottom: parent.bottom
        }
        clip: true
        QC.ScrollBar.vertical: QC.ScrollBar {
            visible: !GuiController.isMobile
        }
        spacing: Style.fromPixel(20)
        model: weekdayFilterModel
        delegate: ColumnLayout {
            id: column
            width: parent ? parent.width : 0

            required property date date
            required property string prettyDate
            required property TaskFilterModel tasks
            required property bool isPast
            required property bool isToday
            required property int index

            spacing: Style.fromPixel(10)
            objectName: "weekday_" + index
            readonly property int taskCount: tasks.count
            // For tests, the same properties as in the Slint UI
            readonly property int count: tasks.count
            readonly property string text: prettyDate

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: Style.fromPixel(6)
                Layout.rightMargin: Style.fromPixel(6)
                Layout.minimumHeight: Style.fromPixel(30)
                spacing: Style.fromPixel(10)

                Text {
                    text: Qt.formatDate(column.date, "dddd")
                    font.pixelSize: Style.fromPixel(18.5)
                    font.weight: Font.DemiBold
                    color: column.isPast ? Style.inkSecondary : Style.ink
                }

                Rectangle {
                    visible: column.isToday
                    implicitWidth: todayText.implicitWidth + Style.fromPixel(16)
                    implicitHeight: todayText.implicitHeight + Style.fromPixel(6)
                    radius: Style.fromPixel(10)
                    color: Style.accentSoft

                    Text {
                        id: todayText
                        anchors.centerIn: parent
                        text: "Today"
                        font.pixelSize: Style.fromPixel(11.5)
                        font.weight: Font.DemiBold
                        color: Style.accentText
                    }
                }

                Text {
                    text: column.count > 0 ? column.count : "Nothing planned"
                    font.pixelSize: Style.fromPixel(column.count > 0 ? 13 : 13.5)
                    color: Style.inkTertiary
                }

                Item {
                    Layout.fillWidth: true
                }

                IconButton {
                    id: addTaskButton
                    objectName: "addTaskButton_" + column.index
                    diameter: Style.fromPixel(30)
                    fillColor: Style.raised
                    fontAwesomeIcon: "\uf067"
                    iconSize: Style.fromPixel(16)
                    iconColor: Style.inkSecondary
                    onClicked: {
                        GuiController.setTaskBeingEdited("", column.date);
                    }
                }
            }

            Rectangle {
                visible: column.count > 0
                Layout.fillWidth: true
                implicitHeight: tasksColumn.implicitHeight + 2
                color: Style.surface
                border.width: 1
                border.color: Style.line
                radius: Style.fromPixel(18)

                Column {
                    id: tasksColumn
                    anchors.fill: parent
                    anchors.margins: 1

                    Repeater {
                        id: taskRepeater
                        model: column.tasks

                        delegate: Column {
                            id: entry

                            required property int index
                            required property string uuid
                            required property string title
                            required property bool isDone
                            required property bool isImportant
                            required property bool isEvening
                            required property var dueDate
                            required property var dueDateRaw
                            required property bool hasDueDate
                            required property string tagName
                            required property bool isFromCalendar
                            required property string calendarName
                            required property bool isSoon
                            required property bool isLater
                            required property bool isCurrent
                            required property bool isDueTomorrow
                            required property bool hasDueDateTime
                            required property string allTags
                            required property bool hasNotes
                            required property bool isGoal
                            required property bool isYearly
                            required property string meetingUrl
                            required property bool isFirstEvening


                            width: tasksColumn.width

                            Rectangle {
                                objectName: "eveningLabel_" + column.index + "_" + entry.index
                                visible: entry.isFirstEvening
                                width: parent.width
                                height: Style.fromPixel(40)
                                color: Style.duskSoft
                                topLeftRadius: entry.index === 0 ? Style.fromPixel(18) - 1 : 0
                                topRightRadius: entry.index === 0 ? Style.fromPixel(18) - 1 : 0

                                Rectangle {
                                    visible: entry.index > 0
                                    x: Style.fromPixel(47)
                                    width: parent.width - x
                                    height: 1
                                    color: Style.line
                                }

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: Style.fromPixel(14)
                                    spacing: Style.fromPixel(8)

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "\uf186"
                                        font.family: eveningFont.name
                                        font.pixelSize: Style.fromPixel(15)
                                        color: Style.dusk
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "This evening"
                                        font.pixelSize: Style.fromPixel(13.5)
                                        font.weight: Font.DemiBold
                                        color: Style.dusk
                                    }
                                }
                            }

                            TaskRow {
                                objectName: "task_" + column.index + "_" + entry.index

                                width: parent.width
                                rowIndex: entry.isFirstEvening ? 0 : entry.index
                                taskUuid: entry.uuid
                                taskTitle: entry.title
                                taskIsDone: entry.isDone
                                taskIsImportant: entry.isImportant
                                taskIsEvening: entry.isEvening
                                taskDueDate: entry.dueDate
                                taskDueDateRaw: entry.dueDateRaw
                                taskHasDueDate: entry.hasDueDate
                                taskHasDueDateTime: entry.hasDueDateTime
                                taskTagName: entry.tagName
                                taskIsFromCalendar: entry.isFromCalendar
                                taskCalendarName: entry.calendarName
                                taskAllTags: entry.allTags
                                taskHasNotes: entry.hasNotes
                                taskIsGoal: entry.isGoal
                                taskIsYearly: entry.isYearly
                                taskMeetingUrl: entry.meetingUrl
                                taskIsSoon: entry.isSoon
                                taskIsLater: entry.isLater
                                taskIsCurrent: entry.isCurrent
                                taskIsDueTomorrow: entry.isDueTomorrow
                                showsDate: false

                                onClicked: {
                                    GuiController.setTaskMenuUuid(entry.uuid);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    FontLoader {
        id: eveningFont
        source: "fonts/FontAwesome7-Solid-900.otf"
    }
}
