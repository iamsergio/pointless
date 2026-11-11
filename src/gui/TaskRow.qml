// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Rectangle {
    id: root

    required property string taskUuid
    required property string taskTitle
    required property bool taskIsDone
    required property bool taskIsImportant
    required property bool taskIsEvening
    required property string taskDueDate
    required property bool taskHasDueDate
    required property bool taskHasDueDateTime
    required property string taskTagName
    required property bool taskIsFromCalendar
    required property string taskCalendarName
    required property string taskAllTags
    required property bool taskHasNotes
    required property bool taskIsGoal
    required property bool taskIsYearly
    required property string taskMeetingUrl

    required property bool taskIsSoon
    required property bool taskIsLater
    required property bool taskIsCurrent
    required property bool taskIsDueTomorrow
    property var taskDueDateRaw
    property int rowIndex: 0
    property bool showTags: true
    property bool showsTagsInSecondLine: false
    property bool showsDate: true

    readonly property bool isPomodoroActive: GuiController.pomodoroController.currentTaskUuid === taskUuid
    readonly property bool useSmallMode: taskIsEvening && !GuiController.isEvening
    readonly property color circleColor: isPomodoroActive ? Style.accent : (taskIsImportant ? Style.amber : Style.inkTertiary)
    readonly property bool hasOtherTags: showsTagsInSecondLine && allTagsExceptCurrent(taskAllTags) !== ""
    readonly property bool hasCalendar: taskIsFromCalendar && taskCalendarName !== ""
    readonly property bool hasTag: showTags && taskTagName !== ""
    readonly property bool hasMeta: !useSmallMode && (hasCalendar || taskHasDueDateTime || hasTag || hasOtherTags)
    readonly property string timeText: taskDueDate.substring(taskDueDate.lastIndexOf(", ") + 2)
    readonly property bool showsLeaf: taskHasDueDate && showsDate && taskDueDateRaw !== undefined
    readonly property string leafMonth: {
        if (!showsLeaf)
            return "";
        const otherYear = /\b\d{4}\b/.test(taskDueDate);
        return Qt.formatDate(taskDueDateRaw, otherYear ? "MMM yyyy" : "MMM");
    }

    implicitHeight: useSmallMode ? Style.fromPixel(42) : Math.max(Style.fromPixel(52), content.implicitHeight + Style.fromPixel(20))
    height: implicitHeight
    color: "transparent"

    readonly property string text: taskTitle

    signal clicked

    function allTagsExceptCurrent(allTags) {
        var tagsArray = allTags.split(",").map(function (tag) {
            return tag.trim();
        });
        var filteredTags = tagsArray.filter(function (tag) {
            return tag !== root.taskTagName;
        });
        return filteredTags.join(", ");
    }

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    HoverHandler {
        id: rowHover
        cursorShape: Qt.PointingHandCursor
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(4)
        radius: Style.fromPixel(12)
        color: root.isPomodoroActive ? Style.accentSoft : Style.raised
        visible: rowHover.hovered || root.isPomodoroActive
    }

    Rectangle {
        visible: root.rowIndex > 0
        x: Style.fromPixel(47)
        width: parent.width - x
        height: 1
        color: Style.line
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
        onPressAndHold: GuiController.setTaskImportant(root.taskUuid, !root.taskIsImportant)
    }

    RowLayout {
        id: content
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            leftMargin: Style.fromPixel(13)
            rightMargin: Style.fromPixel(12)
        }
        spacing: Style.fromPixel(12)

        Item {
            visible: !root.taskIsGoal && !root.taskIsYearly
            implicitWidth: Style.fromPixel(root.useSmallMode ? 18 : 22)
            implicitHeight: implicitWidth

            HoverHandler {
                id: checkHover
                cursorShape: Qt.PointingHandCursor
            }

            Rectangle {
                anchors.fill: parent
                radius: width / 2
                color: root.taskIsDone ? (checkHover.hovered ? Qt.lighter(Style.accent, 1.15) : Style.accent) : (checkHover.hovered ? Style.accentSoft : "transparent")
                border.width: Style.fromPixel(2)
                border.color: root.taskIsDone || checkHover.hovered ? Style.accent : (root.useSmallMode && !root.isPomodoroActive ? Style.dusk : root.circleColor)
            }

            Text {
                anchors.centerIn: parent
                visible: root.taskIsDone || checkHover.hovered
                text: "\uf00c"
                font.family: fontAwesome.name
                font.pixelSize: Style.fromPixel(root.useSmallMode ? 10 : 12)
                color: root.taskIsDone ? Style.accentInk : Style.accentText
            }

            MouseArea {
                anchors.fill: parent
                anchors.margins: -Style.fromPixel(8)
                onClicked: GuiController.taskModel.setTaskDone(root.taskUuid, !root.taskIsDone)
            }
        }

        Text {
            visible: root.taskIsYearly && !root.taskIsGoal
            text: "\uf101"
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(18)
            color: Style.inkTertiary
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: Style.fromPixel(22)

            MouseArea {
                anchors.fill: parent
                anchors.margins: -Style.fromPixel(8)
                onClicked: GuiController.taskModel.advanceYearlyTask(root.taskUuid)
            }
        }

        Text {
            visible: root.taskIsGoal
            text: "\uf11e"
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(18)
            color: Style.inkTertiary
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: Style.fromPixel(22)
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(3)

            Text {
                Layout.fillWidth: true
                text: root.taskTitle
                font.pixelSize: Style.fromPixel(root.useSmallMode ? 14 : 15.5)
                font.weight: Font.Medium
                color: root.taskIsDone ? Style.inkTertiary : (root.useSmallMode ? Style.inkSecondary : Style.ink)
                font.strikeout: root.taskIsDone
                elide: Text.ElideRight
            }

            RowLayout {
                visible: root.hasMeta
                spacing: Style.fromPixel(10)

                RowLayout {
                    visible: root.hasCalendar
                    spacing: Style.fromPixel(4)

                    Text {
                        text: "\uf133"
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(11)
                        color: Style.inkSecondary
                    }

                    Text {
                        text: root.taskCalendarName
                        font.pixelSize: Style.fromPixel(12.5)
                        color: Style.inkSecondary
                    }
                }

                RowLayout {
                    visible: root.taskHasDueDateTime
                    spacing: Style.fromPixel(4)

                    Text {
                        text: "\uf017"
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(11)
                        color: Style.inkSecondary
                    }

                    Text {
                        text: root.timeText
                        font.pixelSize: Style.fromPixel(12.5)
                        color: Style.inkSecondary
                    }
                }

                RowLayout {
                    visible: root.hasTag
                    spacing: Style.fromPixel(5)

                    Rectangle {
                        implicitWidth: Style.fromPixel(7)
                        implicitHeight: Style.fromPixel(7)
                        radius: width / 2
                        color: GuiController.colorFromTag(root.taskTagName)
                    }

                    Text {
                        objectName: "tagText"
                        text: root.taskTagName
                        font.pixelSize: Style.fromPixel(12.5)
                        color: Style.inkSecondary
                    }
                }

                Text {
                    visible: root.hasOtherTags
                    text: root.allTagsExceptCurrent(root.taskAllTags)
                    font.pixelSize: Style.fromPixel(12.5)
                    color: Style.inkSecondary
                }
            }
        }

        Text {
            objectName: "meetingLinkButton"
            visible: root.taskMeetingUrl !== ""
            text: "\uf0c1"
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(16)
            color: meetingLinkHover.hovered ? Style.accent : Style.inkTertiary

            HoverHandler {
                id: meetingLinkHover
                cursorShape: Qt.PointingHandCursor
            }

            MouseArea {
                anchors.fill: parent
                anchors.margins: -Style.fromPixel(8)
                onClicked: Qt.openUrlExternally(root.taskMeetingUrl)
            }
        }

        Text {
            visible: root.taskHasNotes
            text: "\uf249"
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(16)
            color: Style.inkTertiary

            MouseArea {
                anchors.fill: parent
                anchors.margins: -Style.fromPixel(8)
                onClicked: GuiController.openNotesEditor(root.taskUuid)
            }
        }

        Text {
            visible: root.taskIsImportant
            text: "\uf005"
            font.family: fontAwesome.name
            font.pixelSize: Style.fromPixel(16)
            color: Style.amber
        }

        Rectangle {
            visible: root.isPomodoroActive
            implicitHeight: Style.fromPixel(26)
            implicitWidth: pomodoroRow.implicitWidth + Style.fromPixel(20)
            radius: height / 2
            color: Style.accent

            RowLayout {
                id: pomodoroRow
                anchors.centerIn: parent
                spacing: Style.fromPixel(5)

                Text {
                    text: "\uf017"
                    font.family: fontAwesome.name
                    font.pixelSize: Style.fromPixel(12)
                    color: Style.accentInk
                }

                Text {
                    text: GuiController.pomodoroController.remainingTimeText
                    font.pixelSize: Style.fromPixel(12.5)
                    font.weight: Font.DemiBold
                    color: Style.accentInk
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: GuiController.pomodoroController.stop()
            }
        }

        Rectangle {
            visible: root.showsLeaf
            implicitWidth: Math.max(Style.fromPixel(40), leafColumn.implicitWidth + Style.fromPixel(10))
            implicitHeight: leafColumn.implicitHeight + Style.fromPixel(10)
            radius: Style.fromPixel(11)
            color: Style.raised

            ColumnLayout {
                id: leafColumn
                anchors.centerIn: parent
                spacing: 0

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: root.leafMonth
                    font.pixelSize: Style.fromPixel(10.5)
                    font.weight: Font.DemiBold
                    color: Style.inkSecondary
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: root.showsLeaf ? Qt.formatDate(root.taskDueDateRaw, "d") : ""
                    font.pixelSize: Style.fromPixel(17)
                    font.weight: Font.DemiBold
                    color: Style.ink
                }
            }
        }
    }
}
