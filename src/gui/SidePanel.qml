// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Item {
    id: root
    property bool isOpen: false
    visible: isOpen || panelRect.x > -panelRect.width

    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "#80000000"
        opacity: root.isOpen ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.isOpen = false
        }
    }

    Rectangle {
        id: panelRect
        width: Style.fromPixel(250)
        height: parent.height
        color: Style.background
        x: root.isOpen ? 0 : -width

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        ColumnLayout {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: Style.fromPixel(40)
                leftMargin: Style.fromPixel(16)
                rightMargin: Style.fromPixel(16)
            }
            spacing: Style.fromPixel(4)

            Text {
                text: "Pointless"
                color: Style.sectionTextColor
                font.pixelSize: Style.fromPixel(20)
                font.bold: true
                Layout.bottomMargin: Style.fromPixel(16)
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(1)
                color: Style.taskBackground
                Layout.bottomMargin: Style.fromPixel(8)
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Search"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.currentPage = "search";
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "About"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.currentPage = "about";
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Cleanup"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.currentPage = "cleanup";
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Tags"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.currentPage = "tags";
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)
                visible: GuiController.isMobile || GuiController.isMacOS

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Calendars"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.currentPage = "calendars";
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)
                visible: GuiController.isAuthenticated

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Logout"
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.logout();
                        root.isOpen = false;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(40)
                visible: GuiController.isDebug

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Debug"
                    color: "red"
                    font.pixelSize: Style.fromPixel(16)
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        GuiController.dumpDebug();
                        root.isOpen = false;
                    }
                }
            }
        }
    }
}
