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
        function onIsEditingChanged() {
            if (!GuiController.isEditing) {
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

            Keys.onPressed: function (event) {
                if (GuiController.isEditing) {
                    console.warn("Unexpected key press while editing. Ignoring.");
                    return;
                }
            }

            EditTask {
                id: editTaskView
                anchors.fill: parent
                visible: GuiController.isEditing
                z: mainView.z + 1

                onBackClicked: {
                    GuiController.onBackClicked();
                }

                onSaveClicked: function (title, tag, evening) {
                    GuiController.saveTask(title, tag, evening);
                }
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
                    id: addTaskButton
                    fontAwesomeIcon: "\uf055"
                    iconSize: Style.plusButtonSize
                    iconColor: Style.plusButtonColor
                    backgroundColor: "transparent"
                    visible: !GuiController.isEditing
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
                    visible: !GuiController.isEditing && !GuiController.isOfflineMode
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
                }
                z: 300

                MenuItem {
                    id: editMenuItem
                    text: "Edit..."
                    onTriggered: {
                        GuiController.setTaskBeingEdited(GuiController.taskMenuUuid, new Date(NaN));
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
            }

            FontAwesomeButton {
                id: debugButton
                fontAwesomeIcon: "\uf121"
                iconSize: Style.fromPixel(16)
                iconColor: "red"
                backgroundColor: "transparent"
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    leftMargin: Style.fromPixel(32)
                    bottomMargin: Style.fromPixel(32)
                }
                visible: GuiController.isDebug
                z: 100
                onClicked: GuiController.dumpDebug()
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
        }
    }
}
