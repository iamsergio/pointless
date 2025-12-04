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
    visibility: Controller.isMobile ? ApplicationWindow.FullScreen : ApplicationWindow.Windowed
    flags: Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint

    title: "Pointless"
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
                visible: Controller.isEditing
                z: mainView.z + 1

                onBackClicked: {
                    Controller.isEditing = false;
                    Controller.uuidBeingEdited = "";
                }

                onSaveClicked: {
                    // Implement save logic here
                    Controller.isEditing = false;
                    Controller.uuidBeingEdited = "";
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
                    onClicked: Controller.refresh()
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
                    visible: Controller.isDebug
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
                            isActive: Controller.currentViewType === Controller.Week
                            onClicked: Controller.currentViewType = Controller.Week
                        }

                        ViewButton {
                            id: soonViewButton
                            text: "Soon"
                            isActive: Controller.currentViewType === Controller.Soon
                            onClicked: Controller.currentViewType = Controller.Soon
                        }

                        ViewButton {
                            id: laterViewButton
                            text: "Later"
                            isActive: Controller.currentViewType === Controller.Later
                            onClicked: Controller.currentViewType = Controller.Later
                        }
                    }

                    WeekNavigator {
                        id: weekNavigator
                        Layout.fillWidth: true
                        visible: Controller.currentViewType === Controller.Week
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
                        visible: Controller.currentViewType === Controller.Week
                    }

                    SoonView {
                        id: soonView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: Controller.currentViewType === Controller.Soon
                    }

                    LaterView {
                        id: laterView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: Controller.currentViewType === Controller.Later
                    }
                }
            }
        }
    }
}
