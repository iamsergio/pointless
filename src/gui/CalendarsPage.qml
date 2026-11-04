// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Fusion as QC
import QtQuick.Layouts

import pointless 1.0

Page {
    id: root
    pageId: "calendars"
    pageTitle: "Calendars"
    onVisibleChanged: { if (!visible) GuiController.clearFetchCalendarStatusText() }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.fromPixel(14)

        Text {
            Layout.fillWidth: true
            text: "Events from the calendars you turn on appear in your week as tasks."
            color: Style.inkSecondary
            font.pixelSize: Style.fromPixel(14)
            wrapMode: Text.WordWrap
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: listView.contentHeight
            radius: Style.fromPixel(18)
            color: Style.surface
            clip: true

            ListView {
                id: listView
                anchors.fill: parent
                model: GuiController.calendarsModel
                boundsBehavior: Flickable.StopAtBounds

                section.property: "accountName"
                section.delegate: Item {
                    required property string section
                    width: listView.width
                    implicitHeight: section !== "" ? Style.fromPixel(34) : 0
                    visible: section !== ""

                    Text {
                        anchors {
                            left: parent.left
                            leftMargin: Style.fromPixel(16)
                            verticalCenter: parent.verticalCenter
                        }
                        text: parent.section
                        color: Style.inkTertiary
                        font.pixelSize: Style.fromPixel(12)
                        font.weight: Font.DemiBold
                    }
                }

                delegate: Item {
                    id: delegateRoot
                    required property int index
                    required property string calendarId
                    required property string title
                    required property string color
                    required property bool isEnabled
                    required property string accountName

                    width: listView.width
                    implicitHeight: Style.fromPixel(54)

                    RowLayout {
                        anchors {
                            fill: parent
                            leftMargin: Style.fromPixel(16)
                            rightMargin: Style.fromPixel(14)
                        }
                        spacing: Style.fromPixel(12)

                        Rectangle {
                            Layout.preferredWidth: Style.fromPixel(10)
                            Layout.preferredHeight: Style.fromPixel(10)
                            radius: width / 2
                            color: delegateRoot.color
                        }

                        Text {
                            Layout.fillWidth: true
                            text: delegateRoot.title
                            color: Style.ink
                            font.pixelSize: Style.fromPixel(15.5)
                            elide: Text.ElideRight
                        }

                        Switch {
                            checked: delegateRoot.isEnabled
                            onToggled: GuiController.calendarsModel.setEnabled(delegateRoot.calendarId, !delegateRoot.isEnabled)
                        }
                    }

                    Rectangle {
                        visible: delegateRoot.index < listView.count - 1
                        anchors.bottom: parent.bottom
                        x: Style.fromPixel(16)
                        width: parent.width - Style.fromPixel(16)
                        height: 1
                        color: Style.line
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Text {
            Layout.fillWidth: true
            text: GuiController.fetchCalendarStatusText
            color: Style.inkSecondary
            font.pixelSize: Style.fromPixel(13)
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: GuiController.fetchCalendarStatusText !== ""
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(10)

            QC.Button {
                id: fetchButton
                Layout.fillWidth: true
                implicitHeight: Style.fromPixel(48)
                enabled: !GuiController.isFetchingCalendarEvents
                onClicked: GuiController.fetchCalendarEvents()
                background: Rectangle {
                    radius: Style.fromPixel(15)
                    color: Style.accent
                    opacity: !fetchButton.enabled ? 0.5 : (fetchButton.pressed ? 0.8 : 1)
                }
                contentItem: Text {
                    text: GuiController.isFetchingCalendarEvents ? "Fetching..." : "Fetch calendar events"
                    color: Style.accentInk
                    font.pixelSize: Style.fromPixel(15.5)
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            IconButton {
                diameter: Style.fromPixel(48)
                fontAwesomeIcon: "\uf084"
                iconSize: Style.fromPixel(18)
                fillColor: Style.raised
                visible: !GuiController.isMobile && !GuiController.isMacOS && !GuiController.calendarProviderConfigured
                onClicked: GuiController.fetchPassStoreCredentials()
            }
        }
    }
}
