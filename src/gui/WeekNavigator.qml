// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Layouts 1.15

import pointless 1.0

Rectangle {
    color: Style.background
    height: 60

    RowLayout {
        anchors.centerIn: parent
        spacing: Style.fromPixel(5)

        FontAwesomeButton {
            id: leftIcon
            fontAwesomeIcon: "\uf053" // left arrow
            iconSize: Style.fromPixel(14)
            iconColor: Style.textColor2
            backgroundColor: "transparent"
            implicitWidth: Style.fromPixel(40)
            implicitHeight: Style.fromPixel(40)
            onClicked: GuiController.navigatorGotoPreviousWeek()
        }

        MouseArea {
            implicitHeight: dateRangeText.implicitHeight
            implicitWidth: dateRangeText.implicitWidth
            Layout.alignment: Qt.AlignVCenter
            onClicked: GuiController.navigatorGotoToday()
            Text {
                id: dateRangeText
                text: GuiController.navigatorPrettyStartDate + " - " + GuiController.navigatorPrettyEndDate
                font.pixelSize: Style.fromPixel(14)
                color: Style.textColor2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
            }
        }

        FontAwesomeButton {
            id: rightIcon
            fontAwesomeIcon: "\uf054" // right arrow
            iconSize: leftIcon.iconSize
            iconColor: leftIcon.iconColor
            backgroundColor: leftIcon.backgroundColor
            implicitWidth: leftIcon.implicitWidth
            implicitHeight: leftIcon.implicitHeight
            onClicked: GuiController.navigatorGotoNextWeek()
        }
    }
}
