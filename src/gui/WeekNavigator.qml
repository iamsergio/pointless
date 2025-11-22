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
        spacing: 20

        FontAwesomeButton {
            fontAwesomeIcon: "\uf053"
            iconSize: 20
            iconColor: "white"
            implicitWidth: 40
            implicitHeight: 40
            onClicked: Controller.navigatorGotoPreviousWeek()
        }

        MouseArea {
            implicitHeight: dateRangeText.implicitHeight
            implicitWidth: dateRangeText.implicitWidth
            Layout.alignment: Qt.AlignVCenter
            onClicked: Controller.navigatorGotoToday()
            Text {
                id: dateRangeText
                text: Controller.navigatorPrettyStartDate + " - " + Controller.navigatorPrettyEndDate
                font.pixelSize: Style.fromPixel(18)
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
            }
        }

        FontAwesomeButton {
            fontAwesomeIcon: "\uf054"
            iconSize: 20
            iconColor: "white"
            implicitWidth: 40
            implicitHeight: 40
            onClicked: Controller.navigatorGotoNextWeek()
        }
    }
}
