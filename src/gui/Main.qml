// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import pointless 1.0

QC.ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"

    visible: true
    width: Style.fromPixel(640)
    height: Style.fromPixel(640)
    visibility: GuiController.isMobile ? QC.ApplicationWindow.FullScreen : QC.ApplicationWindow.Windowed
    flags: Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint

    title: GuiController.windowTitle
    topPadding: 0
    bottomPadding: 0

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

                onSaveClicked: function (title, tag, evening) {
                    GuiController.saveTask(title, tag, evening);
                }
            }

            AboutPage {
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            TagsPage {
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            TasksByTagPage {
                anchors.fill: parent
                onBackClicked: {
                    GuiController.currentPage = "";
                    GuiController.currentTag = "";
                }
                z: mainView.z + 1
            }

            CleanupPage {
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            CalendarsPage {
                anchors.fill: parent
                onBackClicked: GuiController.currentPage = ""
                z: mainView.z + 1
            }

            SearchPage {
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
                        Layout.alignment: Qt.AlignLeft
                        spacing: Style.fromPixel(10)

                        ViewButton {
                            id: weekViewButton
                            text: "Week"
                            isActive: GuiController.currentViewType === GuiController.Week
                            onClicked: GuiController.currentViewType = GuiController.Week
                        }

                        ViewButton {
                            id: soonViewButton
                            text: "Soon"
                            isActive: GuiController.currentViewType === GuiController.Soon
                            onClicked: GuiController.currentViewType = GuiController.Soon
                        }

                        ViewButton {
                            id: laterViewButton
                            text: "Later"
                            isActive: GuiController.currentViewType === GuiController.Later
                            onClicked: GuiController.currentViewType = GuiController.Later
                        }
                    }

                    WeekNavigator {
                        id: weekNavigator
                        Layout.fillWidth: true
                        visible: GuiController.currentViewType === GuiController.Week
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: Style.fromPixel(1)
                        color: Style.taskBackground
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
                }
            }

            Item {
                id: bottomArea

                height: addTaskButton.height
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: Style.fromPixel(GuiController.isMobile ? 32 : 16)
                }

                FontAwesomeButton {
                    id: burgerButton
                    fontAwesomeIcon: "\uf0c9"
                    iconSize: Style.plusButtonSize
                    iconColor: Style.plusButtonColor
                    backgroundColor: "transparent"
                    visible: !GuiController.isEditingTask
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        leftMargin: Style.fromPixel(32)
                    }
                    z: 100
                    onClicked: sidePanel.isOpen = true
                }

                ToggleButton {
                    id: eveningToggle
                    visible: !GuiController.isEditingTask && GuiController.showEveningToggle && GuiController.currentViewType === GuiController.Week
                    toggled: GuiController.showImmediateOnly
                    anchors {
                        left: burgerButton.right
                        bottom: parent.bottom
                        leftMargin: Style.fromPixel(16)
                        bottomMargin: Style.fromPixel(4)
                    }
                    z: 100
                    onToggledChanged: GuiController.showImmediateOnly = toggled
                }

                FontAwesomeButton {
                    id: addTaskButton
                    fontAwesomeIcon: "\uf055"
                    iconSize: Style.plusButtonSize
                    iconColor: Style.plusButtonColor
                    backgroundColor: "transparent"
                    visible: !GuiController.isEditingTask
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        rightMargin: Style.fromPixel(32)
                    }
                    z: 100
                    onClicked: {
                        GuiController.setTaskBeingEdited("", new Date(NaN));
                    }
                }

                FontAwesomeButton {
                    id: refreshButton
                    fontAwesomeIcon: "\uf021"
                    iconSize: Style.plusButtonSize
                    iconColor: Style.plusButtonColor
                    backgroundColor: "transparent"
                    visible: !GuiController.isEditingTask && !GuiController.isOfflineMode
                    enabled: !GuiController.isRefreshing
                    anchors {
                        right: addTaskButton.left
                        bottom: parent.bottom
                        rightMargin: Style.fromPixel(16)
                    }
                    z: 100
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

            Menu {
                id: taskMenu
                visible: GuiController.taskMenuVisible
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: Style.fromPixel(7)
                    rightMargin: Style.fromPixel(7)
                    bottomMargin: Style.fromPixel(7)
                }
                z: mainView.z + 5

                MenuItem {
                    id: editMenuItem
                    text: "Edit..."
                    onTriggered: {
                        GuiController.setTaskBeingEdited(GuiController.taskMenuUuid, new Date(NaN));
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: editNotesMenuItem
                    text: "Edit notes..."
                    onTriggered: {
                        GuiController.openNotesEditor(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: playPomodoroMenuItem
                    visible: GuiController.playPomodoroVisible
                    text: GuiController.pomodoroController.currentTaskUuid === GuiController.taskMenuUuid ? "Stop" : "Play"
                    onTriggered: {
                        if (GuiController.pomodoroController.currentTaskUuid === GuiController.taskMenuUuid) {
                            GuiController.pomodoroController.stop();
                        } else {
                            GuiController.pomodoroController.play(GuiController.taskMenuUuid);
                        }
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToCurrentMenuItem
                    text: "Move to Current"
                    visible: GuiController.moveToCurrentVisible
                    onTriggered: {
                        GuiController.moveTaskToCurrent(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToSoonMenuItem
                    text: "Move to Soon"
                    visible: GuiController.moveToSoonVisible
                    onTriggered: {
                        GuiController.moveTaskToSoon(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToLaterMenuItem
                    text: "Move to Later"
                    visible: GuiController.moveToLaterVisible
                    onTriggered: {
                        GuiController.moveTaskToLater(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToTomorrowMenuItem
                    visible: GuiController.moveToTomorrowVisible
                    text: "Move to Tomorrow"
                    onTriggered: {
                        GuiController.moveTaskToTomorrow(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToMondayMenuItem
                    text: "Move to Next Monday"
                    onTriggered: {
                        GuiController.moveTaskToNextMonday(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: moveToEveningMenuItem
                    text: "Move to Evening"
                    visible: GuiController.moveToEveningVisible
                    onTriggered: {
                        GuiController.moveTaskToEvening(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
                MenuItem {
                    id: debugJsonMenuItem
                    text: "Debug JSON"
                    visible: GuiController.debugMode
                    onTriggered: {
                        GuiController.dumpTaskDebug(GuiController.taskMenuUuid);
                        GuiController.setTaskMenuUuid("");
                    }
                }
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
