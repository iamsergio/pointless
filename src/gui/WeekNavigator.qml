// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Rectangle {
    id: root

    readonly property date startDate: GuiController.navigatorStartDate
    readonly property date endDate: GuiController.navigatorEndDate
    readonly property string titleText: {
        if (startDate.getMonth() === endDate.getMonth())
            return Qt.formatDate(startDate, "MMMM");
        return Qt.formatDate(startDate, "MMM") + " – " + Qt.formatDate(endDate, "MMM");
    }
    readonly property int weekNumber: {
        const d = new Date(Date.UTC(startDate.getFullYear(), startDate.getMonth(), startDate.getDate()));
        const dayNum = d.getUTCDay() || 7;
        d.setUTCDate(d.getUTCDate() + 4 - dayNum);
        const yearStart = new Date(Date.UTC(d.getUTCFullYear(), 0, 1));
        return Math.ceil(((d - yearStart) / 86400000 + 1) / 7);
    }

    color: Style.background
    implicitHeight: content.implicitHeight

    // For tests, the same item as in the Slint UI
    Text {
        id: dateRangeText
        visible: false
        text: GuiController.navigatorPrettyStartDate + " - " + GuiController.navigatorPrettyEndDate
    }

    RowLayout {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: Style.fromPixel(10)

        MouseArea {
            implicitHeight: titleRow.implicitHeight
            implicitWidth: titleRow.implicitWidth
            onClicked: GuiController.navigatorGotoToday()

            RowLayout {
                id: titleRow
                spacing: Style.fromPixel(10)

                Text {
                    Layout.alignment: Qt.AlignBaseline
                    text: root.titleText
                    font.pixelSize: Style.fromPixel(30)
                    font.bold: true
                    color: Style.ink
                }

                Text {
                    Layout.alignment: Qt.AlignBaseline
                    text: "Week " + root.weekNumber
                    font.pixelSize: Style.fromPixel(13.5)
                    color: Style.inkSecondary
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        IconButton {
            id: leftIcon
            diameter: Style.fromPixel(32)
            fontAwesomeIcon: "\uf053"
            iconSize: Style.fromPixel(16)
            onClicked: GuiController.navigatorGotoPreviousWeek()
        }

        IconButton {
            id: rightIcon
            diameter: Style.fromPixel(32)
            fontAwesomeIcon: "\uf054"
            iconSize: Style.fromPixel(16)
            onClicked: GuiController.navigatorGotoNextWeek()
        }
    }
}
