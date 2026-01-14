// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import pointless 1.0

ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"

    visible: true
    width: Style.fromPixel(640)
    height: Style.fromPixel(640)
    visibility: GuiController.isMobile ? ApplicationWindow.FullScreen : ApplicationWindow.Windowed
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
                    GuiController.addNewTask(title, tag, evening);
                }
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

                LoginScreen {
                    id: loginScreen
                    anchors.fill: parent
                    visible: false
                    z: 200
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
                    visible: !GuiController.isEditing
                    anchors {
                        right: addTaskButton.left
                        bottom: parent.bottom
                        rightMargin: Style.fromPixel(16)
                    }
                    z: 100
                    onClicked: GuiController.refresh()
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
        }
    }
}
