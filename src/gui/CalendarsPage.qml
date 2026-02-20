// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "calendars"
    pageTitle: "Calendars"
    onVisibleChanged: { if (!visible) GuiController.clearFetchCalendarStatusText() }

    ListView {
        id: listView
        anchors {
            fill: parent
            bottomMargin: Style.fromPixel(60)
        }
        model: GuiController.calendarsModel
        spacing: Style.fromPixel(2)

        delegate: Item {
            id: delegateRoot
            required property string calendarId
            required property string title
            required property string color
            required property bool isEnabled

            width: listView.width
            implicitHeight: Style.fromPixel(44)

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Style.fromPixel(8)
                    rightMargin: Style.fromPixel(8)
                }
                spacing: Style.fromPixel(8)

                Rectangle {
                    radius: width / 2
                    color: delegateRoot.color
                }

                Text {
                    Layout.fillWidth: true
                    text: delegateRoot.title
                    color: Style.taskTextColor
                    font.pixelSize: Style.fromPixel(16)
                    elide: Text.ElideRight
                }

                CheckBox {
                    checked: delegateRoot.isEnabled
                    onToggled: {
                        GuiController.calendarsModel.setEnabled(delegateRoot.calendarId, checked);
                    }
                }
            }
        }
    }

    Text {
        id: statusText
        anchors {
            bottom: fetchButton.top
            left: parent.left
            right: parent.right
            margins: Style.fromPixel(8)
        }
        text: GuiController.fetchCalendarStatusText
        color: Style.taskTextColor
        font.pixelSize: Style.fromPixel(14)
        horizontalAlignment: Text.AlignHCenter
        visible: GuiController.fetchCalendarStatusText !== ""
    }

    Rectangle {
        id: fetchButton
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Style.fromPixel(8)
        }
        height: Style.fromPixel(44)
        color: GuiController.isFetchingCalendarEvents ? Qt.darker(Style.buttonColor, 1.3) : Style.buttonColor
        radius: Style.fromPixel(8)

        Row {
            anchors.centerIn: parent
            spacing: Style.fromPixel(8)

            FontAwesomeButton {
                fontAwesomeIcon: "\uf021"
                iconSize: Style.fromPixel(16)
                iconColor: Style.buttonTextColor
                backgroundColor: "transparent"
                visible: GuiController.isFetchingCalendarEvents
                enabled: false
                anchors.verticalCenter: parent.verticalCenter

                RotationAnimation on rotation {
                    from: 0
                    to: 360
                    duration: 1000
                    loops: Animation.Infinite
                    running: GuiController.isFetchingCalendarEvents
                }
            }

            Text {
                text: GuiController.isFetchingCalendarEvents ? "Fetching..." : "Fetch Calendar Events"
                color: Style.buttonTextColor
                font.pixelSize: Style.fromPixel(16)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: !GuiController.isFetchingCalendarEvents
            onClicked: GuiController.fetchCalendarEvents()
        }
    }
}
