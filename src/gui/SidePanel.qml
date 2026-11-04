// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Item {
    id: root
    property bool isOpen: false
    visible: isOpen || panelRect.x > -panelRect.width

    FontLoader {
        id: fontAwesome
        source: "fonts/FontAwesome7-Solid-900.otf"
    }

    component PanelRow: Rectangle {
        id: row

        readonly property string iconFont: fontAwesome.name

        property string fontAwesomeIcon: ""
        property alias text: label.text
        property color textColor: Style.ink
        property color iconColor: Style.inkSecondary
        property alias trailing: trailingSlot.data

        signal clicked

        Layout.fillWidth: true
        implicitHeight: Style.fromPixel(48)
        radius: Style.fromPixel(14)
        color: rowArea.containsPress ? Style.raised : "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Style.fromPixel(14)
            anchors.rightMargin: Style.fromPixel(14)
            spacing: Style.fromPixel(14)

            Text {
                text: row.fontAwesomeIcon
                font.family: row.iconFont
                font.pixelSize: Style.fromPixel(20)
                color: row.iconColor
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: Style.fromPixel(24)
            }

            Text {
                id: label
                font.pixelSize: Style.fromPixel(15.5)
                color: row.textColor
                Layout.fillWidth: true
            }

            Item {
                id: trailingSlot
                implicitWidth: childrenRect.width
                implicitHeight: childrenRect.height
            }
        }

        MouseArea {
            id: rowArea
            anchors.fill: parent
            onClicked: row.clicked()
        }
    }

    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "#8C000000"
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
        width: Math.min(Style.fromPixel(270), parent.width * 0.75)
        height: parent.height
        color: Style.surface
        topRightRadius: Style.fromPixel(30)
        bottomRightRadius: Style.fromPixel(30)
        x: root.isOpen ? 0 : -width

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        MouseArea {
            anchors.fill: parent
        }

        ColumnLayout {
            anchors {
                fill: parent
                topMargin: Style.fromPixel(28) + root.SafeArea.margins.top
                bottomMargin: Style.fromPixel(20) + root.SafeArea.margins.bottom
                leftMargin: Style.fromPixel(14)
                rightMargin: Style.fromPixel(14)
            }
            spacing: Style.fromPixel(4)

            RowLayout {
                Layout.leftMargin: Style.fromPixel(14)
                Layout.bottomMargin: Style.fromPixel(18)
                spacing: Style.fromPixel(12)

                Rectangle {
                    implicitWidth: Style.fromPixel(30)
                    implicitHeight: Style.fromPixel(30)
                    radius: width / 2
                    color: Style.accent

                    Text {
                        anchors.centerIn: parent
                        text: "\uf00c"
                        font.family: fontAwesome.name
                        font.pixelSize: Style.fromPixel(15)
                        color: Style.accentInk
                    }
                }

                Text {
                    text: "Pointless"
                    color: Style.ink
                    font.pixelSize: Style.fromPixel(24)
                    font.bold: true
                }
            }

            PanelRow {
                objectName: "sidePanelSearch"
                text: "Search"
                fontAwesomeIcon: "\uf002"
                onClicked: {
                    GuiController.currentPage = "search";
                    root.isOpen = false;
                }
            }

            PanelRow {
                objectName: "sidePanelTags"
                text: "Tags"
                fontAwesomeIcon: "\uf02b"
                onClicked: {
                    GuiController.currentPage = "tags";
                    root.isOpen = false;
                }
            }

            PanelRow {
                objectName: "sidePanelCalendars"
                text: "Calendars"
                fontAwesomeIcon: "\uf073"
                onClicked: {
                    GuiController.currentPage = "calendars";
                    root.isOpen = false;
                }
            }

            PanelRow {
                objectName: "sidePanelCleanup"
                text: "Cleanup"
                fontAwesomeIcon: "\uf51a"
                onClicked: {
                    GuiController.currentPage = "cleanup";
                    root.isOpen = false;
                }
            }

            PanelRow {
                objectName: "sidePanelAbout"
                text: "About"
                fontAwesomeIcon: "\uf05a"
                onClicked: {
                    GuiController.currentPage = "about";
                    root.isOpen = false;
                }
            }

            Item {
                Layout.fillHeight: true
            }

            PanelRow {
                visible: GuiController.isAuthenticated
                text: "Log out"
                fontAwesomeIcon: "\uf2f5"
                onClicked: {
                    GuiController.logout();
                    root.isOpen = false;
                }
            }

            PanelRow {
                visible: GuiController.isDebug
                text: "Debug"
                fontAwesomeIcon: "\uf188"
                onClicked: {
                    GuiController.dumpDebug();
                    root.isOpen = false;
                }

                trailing: Rectangle {
                    width: pillLabel.implicitWidth + Style.fromPixel(16)
                    height: Style.fromPixel(22)
                    radius: height / 2
                    color: Style.raised

                    Text {
                        id: pillLabel
                        anchors.centerIn: parent
                        text: "Dev build"
                        font.pixelSize: Style.fromPixel(11)
                        font.weight: Font.DemiBold
                        color: Style.inkSecondary
                    }
                }
            }
        }
    }
}
