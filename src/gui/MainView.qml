// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Item {
    id: root
    objectName: "pointlessMainView"

    Connections {
        target: GuiController
        function onIsEditingTaskChanged() {
            if (!GuiController.isEditingTask) {
                withinSafeArea.forceActiveFocus();
            }
        }
    }

    Component.onCompleted: {
        withinSafeArea.forceActiveFocus();
    }

    Rectangle {
        anchors.fill: parent
        color: Style.background

        Item {
            id: withinSafeArea
            anchors.fill: parent
            anchors.topMargin: parent.SafeArea.margins.top

            EditTaskPage {
                id: editTaskView
                anchors.fill: parent
                visible: GuiController.isEditingTask
                z: mainView.z + 2

                onBackClicked: {
                    GuiController.onBackClicked();
                }

                onSaveClicked: function (title, tag, evening, isGoal, isYearly) {
                    GuiController.saveTask(title, tag, evening, isGoal, isYearly);
                }
            }

            AboutPage {
                objectName: "aboutPage"
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            TagsPage {
                objectName: "tagsPage"
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            TasksByTagPage {
                objectName: "tasksByTagPage"
                anchors.fill: parent
                onBackClicked: {
                    GuiController.currentPage = "";
                    GuiController.currentTag = "";
                }
                z: mainView.z + 1
            }

            CleanupPage {
                objectName: "cleanupPage"
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            CalendarsPage {
                objectName: "calendarsPage"
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            SearchPage {
                objectName: "searchPage"
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            LoginScreen {
                id: loginScreen
                anchors.fill: parent
                visible: !GuiController.isAuthenticated && !GuiController.isOfflineMode
                z: 200
            }

            Item {
                id: mainView
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: bottomArea.top
                    bottomMargin: Style.fromPixel(8)
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Style.fromPixel(20)

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Style.fromPixel(50)
                        visible: !GuiController.isEditingTask
                        spacing: Style.fromPixel(4)

                        IconButton {
                            id: burgerButton
                            fontAwesomeIcon: "\uf0c9"
                            onClicked: sidePanel.isOpen = true
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        IconButton {
                            id: searchButton
                            fontAwesomeIcon: "\uf002"
                            onClicked: GuiController.currentPage = "search"
                        }

                        IconButton {
                            id: eveningToggle
                            fontAwesomeIcon: "\uf186"
                            visible: GuiController.currentViewType === GuiController.Week && GuiController.showEveningToggle
                            fillColor: GuiController.showImmediateOnly ? Style.accentSoft : "transparent"
                            iconColor: GuiController.showImmediateOnly ? Style.accentText : Style.inkSecondary
                            onClicked: GuiController.showImmediateOnly = !GuiController.showImmediateOnly
                        }

                        IconButton {
                            id: refreshButton
                            fontAwesomeIcon: "\uf021"
                            visible: !GuiController.isOfflineMode
                            enabled: !GuiController.isRefreshing
                            onClicked: GuiController.refresh()

                            rotation: GuiController.isRefreshing ? 360 : 0

                            RotationAnimation on rotation {
                                id: spinAnimation
                                from: 0
                                to: 360
                                duration: 1000
                                loops: Animation.Infinite
                                running: GuiController.isRefreshing
                            }

                            Behavior on rotation {
                                enabled: !GuiController.isRefreshing
                                NumberAnimation {
                                    duration: 200
                                }
                            }
                        }
                    }

                    WeekNavigator {
                        id: weekNavigator
                        Layout.fillWidth: true
                        visible: GuiController.currentViewType === GuiController.Week
                    }

                    WeekView {
                        id: weekView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: GuiController.currentViewType === GuiController.Week
                    }

                    SoonView {
                        id: soonView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: GuiController.currentViewType === GuiController.Soon
                    }

                    LaterView {
                        id: laterView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: GuiController.currentViewType === GuiController.Later
                    }

                    GoalsView {
                        id: goalsView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: GuiController.currentViewType === GuiController.Goals
                    }
                }
            }

            RowLayout {
                id: bottomArea
                visible: !GuiController.isEditingTask
                height: Style.fromPixel(60)
                spacing: Style.fromPixel(10)
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: Style.fromPixel(12)
                    rightMargin: Style.fromPixel(12)
                    bottomMargin: Style.fromPixel(GuiController.isMobile ? 32 : 16)
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: height / 2
                    color: Style.surface
                    border.width: 1
                    border.color: Style.line

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Style.fromPixel(5)
                        spacing: 0

                        ViewButton {
                            id: weekViewButton
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            text: "Week"
                            fontAwesomeIcon: "\uf784"
                            isActive: GuiController.currentViewType === GuiController.Week
                            onClicked: GuiController.currentViewType = GuiController.Week
                        }

                        ViewButton {
                            id: soonViewButton
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            text: "Soon"
                            fontAwesomeIcon: "\uf04e"
                            isActive: GuiController.currentViewType === GuiController.Soon
                            onClicked: GuiController.currentViewType = GuiController.Soon
                        }

                        ViewButton {
                            id: laterViewButton
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            text: "Later"
                            fontAwesomeIcon: "\uf254"
                            isActive: GuiController.currentViewType === GuiController.Later
                            onClicked: GuiController.currentViewType = GuiController.Later
                        }

                        ViewButton {
                            id: goalsViewButton
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            text: "Goals"
                            fontAwesomeIcon: "\uf024"
                            isActive: GuiController.currentViewType === GuiController.Goals
                            onClicked: GuiController.currentViewType = GuiController.Goals
                        }
                    }
                }

                IconButton {
                    id: addTaskButton
                    diameter: Style.fromPixel(60)
                    fontAwesomeIcon: "\u002b"
                    iconSize: Style.fromPixel(24)
                    iconColor: Style.accentInk
                    fillColor: Style.accent
                    onClicked: {
                        GuiController.setTaskBeingEdited("", new Date(NaN));
                    }
                }
            }

            Menu {
                id: taskMenu
                visible: GuiController.taskMenuVisible
                anchors.fill: parent
                z: mainView.z + 5

                onDismissed: GuiController.setTaskMenuUuid("")

                moveItems: [
                    MenuItem {
                        id: moveToCurrentMenuItem
                        chip: true
                        text: "Current"
                        fontAwesomeIcon: "\uf0e7"
                        visible: GuiController.moveToCurrentVisible
                        onTriggered: {
                            GuiController.moveTaskToCurrent(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: moveToSoonMenuItem
                        chip: true
                        text: "Soon"
                        fontAwesomeIcon: "\uf04e"
                        visible: GuiController.moveToSoonVisible
                        onTriggered: {
                            GuiController.moveTaskToSoon(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: moveToLaterMenuItem
                        chip: true
                        text: "Later"
                        fontAwesomeIcon: "\uf254"
                        visible: GuiController.moveToLaterVisible
                        onTriggered: {
                            GuiController.moveTaskToLater(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: moveToTomorrowMenuItem
                        chip: true
                        text: "Tomorrow"
                        fontAwesomeIcon: "\uf185"
                        visible: GuiController.moveToTomorrowVisible
                        onTriggered: {
                            GuiController.moveTaskToTomorrow(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: moveToMondayMenuItem
                        chip: true
                        text: "Next Monday"
                        fontAwesomeIcon: "\uf784"
                        onTriggered: {
                            GuiController.moveTaskToNextMonday(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: moveToEveningMenuItem
                        chip: true
                        text: "This evening"
                        fontAwesomeIcon: "\uf186"
                        visible: GuiController.moveToEveningVisible
                        onTriggered: {
                            GuiController.moveTaskToEvening(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    }
                ]

                actionItems: [
                    MenuItem {
                        id: editMenuItem
                        text: "Edit task"
                        fontAwesomeIcon: "\uf304"
                        onTriggered: {
                            GuiController.setTaskBeingEdited(GuiController.taskMenuUuid, new Date(NaN));
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: editNotesMenuItem
                        text: "Edit notes"
                        fontAwesomeIcon: "\uf249"
                        onTriggered: {
                            GuiController.openNotesEditor(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: playPomodoroMenuItem
                        visible: GuiController.playPomodoroVisible
                        text: GuiController.pomodoroController.currentTaskUuid === GuiController.taskMenuUuid ? "Stop timer" : "Start timer"
                        fontAwesomeIcon: "\uf2f2"
                        onTriggered: {
                            if (GuiController.pomodoroController.currentTaskUuid === GuiController.taskMenuUuid) {
                                GuiController.pomodoroController.stop();
                            } else {
                                GuiController.pomodoroController.play(GuiController.taskMenuUuid);
                            }
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: deleteTaskMenuItem
                        text: "Delete task"
                        fontAwesomeIcon: "\uf1f8"
                        iconColor: Style.danger
                        textColor: Style.danger
                        visible: GuiController.deleteTaskVisible
                        onTriggered: {
                            GuiController.deleteTask(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    },
                    MenuItem {
                        id: debugJsonMenuItem
                        text: "Debug JSON"
                        fontAwesomeIcon: "\uf188"
                        visible: GuiController.debugMode
                        onTriggered: {
                            GuiController.dumpTaskDebug(GuiController.taskMenuUuid);
                            GuiController.setTaskMenuUuid("");
                        }
                    }
                ]
            }

            ErrorPopup {
                id: errorPopup
                anchors.fill: parent
                visible: GuiController.errorController.errorText.length > 0
                z: 400
                onOkClicked: {
                    GuiController.errorController.errorText = "";
                }
            }

            NotesPopup {
                id: notesPopup
                anchors.fill: parent
                visible: GuiController.isEditingNotes
                z: 450
            }

            SidePanel {
                id: sidePanel
                anchors.fill: parent
                z: 500
            }
        }
    }
}
