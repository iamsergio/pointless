// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls.Fusion as QC
import QtQuick.Layouts 1.15
import pointless 1.0

Page {
    id: root
    objectName: "editTask"
    pageTitle: isNewTask ? "New task" : "Edit task"
    backButtonText: "Cancel"
    rightButtonText: "Save"
    rightButtonObjectName: "saveButton"

    signal saveClicked(string title, string tag, bool evening, bool isGoal, bool isYearly)

    property string selectedTag: ""
    property bool isEvening: false
    property bool isGoal: false
    property bool isYearly: false
    property bool isNewTask: true

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    onVisibleChanged: {
        if (visible) {
            isNewTask = GuiController.titleInEditor === "";
            titleInput.text = GuiController.titleInEditor;
            titleInput.forceActiveFocus();
            root.selectedTag = GuiController.tagInEditor;
            root.isEvening = GuiController.isEveningInEditor;
            root.isGoal = GuiController.isGoalInEditor;
            root.isYearly = GuiController.isYearlyInEditor;
            calendar.setInitialMonth();
            scroller.contentY = 0;
        }
    }

    onRightButtonClicked: root.saveClicked(titleInput.text, root.selectedTag, root.isEvening, root.isGoal, root.isYearly)

    Flickable {
        id: scroller
        anchors.fill: parent
        contentWidth: width
        contentHeight: formColumn.implicitHeight + Style.fromPixel(16)
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        ColumnLayout {
            id: formColumn
            width: scroller.width
            spacing: Style.fromPixel(22)

            QC.TextField {
                id: titleInput
                objectName: "titleInput"
                focus: true
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(56)
                placeholderText: "What needs doing?"
                placeholderTextColor: Style.inkTertiary
                color: Style.ink
                font.pixelSize: Style.fromPixel(23)
                font.weight: Font.DemiBold
                leftPadding: 0
                rightPadding: 0
                selectByMouse: true
                background: Rectangle {
                    color: "transparent"

                    Rectangle {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        height: 2
                        color: Style.accent
                    }
                }
                onAccepted: {
                    if (text.trim() !== "")
                        root.saveClicked(text, root.selectedTag, root.isEvening, root.isGoal, root.isYearly);
                }

                property var completionCandidates: []
                property int completionIndex: -1
                property int completionStart: -1
                property string completedText: ""

                function cycleTagCompletion(step: int): bool {
                    const cycling = completionIndex >= 0 && text === completedText && cursorPosition === completionStart + completionCandidates[completionIndex].length;
                    if (!cycling) {
                        const beforeCursor = text.substring(0, cursorPosition);
                        let wordStart = beforeCursor.lastIndexOf(" ") + 1;
                        if (beforeCursor.charAt(wordStart) === ":")
                            wordStart++;
                        const prefix = beforeCursor.substring(wordStart);
                        if (prefix === "")
                            return false;
                        const candidates = GuiController.tagCompletions(prefix);
                        if (candidates.length === 0)
                            return true;
                        completionCandidates = candidates;
                        completionStart = wordStart;
                        completionIndex = step > 0 ? 0 : candidates.length - 1;
                    } else {
                        completionIndex = (completionIndex + step + completionCandidates.length) % completionCandidates.length;
                    }

                    const candidate = completionCandidates[completionIndex];
                    text = text.substring(0, completionStart) + candidate + text.substring(cursorPosition);
                    cursorPosition = completionStart + candidate.length;
                    completedText = text;
                    return true;
                }

                Keys.onTabPressed: event => {
                    event.accepted = cycleTagCompletion(1);
                }
                Keys.onBacktabPressed: event => {
                    event.accepted = cycleTagCompletion(-1);
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Style.fromPixel(10)

                Text {
                    text: "Tags"
                    color: Style.inkSecondary
                    font.pixelSize: Style.fromPixel(13)
                    font.weight: Font.DemiBold
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: Style.fromPixel(8)

                    Repeater {
                        model: GuiController.tagFilterModel

                        Rectangle {
                            id: chip
                            required property string name
                            objectName: "tag_" + name
                            readonly property bool selected: root.selectedTag === name

                            visible: name !== ""
                            height: Style.fromPixel(34)
                            width: chipRow.implicitWidth + Style.fromPixel(28)
                            radius: height / 2
                            color: selected ? Style.accentSoft : Style.surface
                            border.width: selected ? 0 : 1
                            border.color: Style.line

                            Row {
                                id: chipRow
                                anchors.centerIn: parent
                                spacing: Style.fromPixel(7)

                                Rectangle {
                                    width: Style.fromPixel(7)
                                    height: width
                                    radius: width / 2
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: GuiController.colorFromTag(chip.name)
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: chip.name
                                    color: chip.selected ? Style.accentText : Style.ink
                                    font.pixelSize: Style.fromPixel(14)
                                    font.weight: chip.selected ? Font.DemiBold : Font.Normal
                                }

                                Text {
                                    visible: chip.selected
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "\uf00c"
                                    font.family: fontAwesome.name
                                    font.pixelSize: Style.fromPixel(11)
                                    color: Style.accentText
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.selectedTag = chip.name
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: optionsColumn.implicitHeight
                radius: Style.fromPixel(18)
                color: Style.surface
                clip: true

                ColumnLayout {
                    id: optionsColumn
                    width: parent.width
                    spacing: 0

                    component OptionRow: Item {
                        id: optionRow
                        property string icon: ""
                        property string title: ""
                        property string hint: ""
                        property color tileColor: Style.raised
                        property color tileInk: Style.inkSecondary
                        property bool showDivider: true
                        property alias checked: optionSwitch.checked
                        property alias switchObjectName: optionSwitch.objectName
                        signal toggled

                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(64)

                        Rectangle {
                            id: tile
                            x: Style.fromPixel(14)
                            anchors.verticalCenter: parent.verticalCenter
                            width: Style.fromPixel(32)
                            height: width
                            radius: Style.fromPixel(10)
                            color: optionRow.tileColor

                            Text {
                                anchors.centerIn: parent
                                text: optionRow.icon
                                font.family: fontAwesome.name
                                font.pixelSize: Style.fromPixel(14)
                                color: optionRow.tileInk
                            }
                        }

                        Column {
                            anchors {
                                left: tile.right
                                leftMargin: Style.fromPixel(12)
                                right: optionSwitch.left
                                rightMargin: Style.fromPixel(12)
                                verticalCenter: parent.verticalCenter
                            }
                            spacing: Style.fromPixel(2)

                            Text {
                                width: parent.width
                                text: optionRow.title
                                color: Style.ink
                                font.pixelSize: Style.fromPixel(15.5)
                                elide: Text.ElideRight
                            }

                            Text {
                                width: parent.width
                                text: optionRow.hint
                                color: Style.inkSecondary
                                font.pixelSize: Style.fromPixel(12.5)
                                elide: Text.ElideRight
                            }
                        }

                        Switch {
                            id: optionSwitch
                            anchors {
                                right: parent.right
                                rightMargin: Style.fromPixel(14)
                                verticalCenter: parent.verticalCenter
                            }
                            onToggled: optionRow.toggled()
                        }

                        Rectangle {
                            visible: optionRow.showDivider
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: 1
                            color: Style.line
                        }
                    }

                    OptionRow {
                        icon: "\uf186"
                        title: "Evening"
                        hint: "Shown smaller until the evening"
                        tileColor: Style.duskSoft
                        tileInk: Style.dusk
                        switchObjectName: "eveningCheckBox"
                        checked: root.isEvening
                        onToggled: root.isEvening = !root.isEvening
                    }

                    OptionRow {
                        icon: "\uf024"
                        title: "Goal"
                        hint: "Listed under Goals"
                        switchObjectName: "goalCheckBox"
                        checked: root.isGoal
                        onToggled: root.isGoal = !root.isGoal
                    }

                    OptionRow {
                        icon: "\uf363"
                        title: "Repeats every year"
                        hint: "Comes back on the same date"
                        showDivider: false
                        switchObjectName: "yearlyCheckBox"
                        checked: root.isYearly
                        onToggled: root.isYearly = !root.isYearly
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Style.fromPixel(10)

                Text {
                    text: "Date"
                    color: Style.inkSecondary
                    font.pixelSize: Style.fromPixel(13)
                    font.weight: Font.DemiBold
                }

                Calendar {
                    id: calendar
                    currentDate: GuiController.dateInEditor
                    Layout.fillWidth: true
                    onDateSelected: function (selectedDate) {
                        GuiController.setDateInEditor(selectedDate);
                    }
                }
            }
        }
    }
}
