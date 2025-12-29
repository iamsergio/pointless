// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"

    visible: true
    width: Style.fromPixel(640)
    height: Style.fromPixel(480)
    visibility: GuiController.isMobile ? ApplicationWindow.FullScreen : ApplicationWindow.Windowed
    flags: Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint

    title: GuiController.windowTitle
    topPadding: 0
    bottomPadding: 0

    Rectangle {
        anchors.fill: parent
        color: Style.background

        Item {
            id: withinSafeArea
            anchors.fill: parent
            anchors.topMargin: parent.SafeArea.margins.top

            EditTask {
                id: editTaskView
                anchors.fill: parent
                visible: GuiController.isEditing
                z: mainView.z + 1

                onBackClicked: {
                    GuiController.isEditing = false;
                    GuiController.uuidBeingEdited = "";
                }

                onSaveClicked: {
                    // Implement save logic here
                    GuiController.isEditing = false;
                    GuiController.uuidBeingEdited = "";
                }
            }

            Item {
                id: mainView
                anchors.fill: parent

                LoginScreen {
                    id: loginScreen
                    anchors.fill: parent
                    visible: false
                    z: 200
                }

                // Overlaid refresh button
                FontAwesomeButton {
                    id: refreshButton
                    fontAwesomeIcon: "\uf021" // FontAwesome refresh icon
                    iconSize: Style.fromPixel(16)
                    iconColor: Style.iconColor
                    backgroundColor: "transparent"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: Style.fromPixel(16)
                    anchors.rightMargin: Style.fromPixel(16)
                    z: 100
                    onClicked: GuiController.refresh()
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
                        leftMargin: Style.fromPixel(16)
                        bottomMargin: Style.fromPixel(16)
                    }
                    visible: GuiController.isDebug
                    z: 100
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
        }
    }
}
