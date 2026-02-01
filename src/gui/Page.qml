// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Rectangle {
    id: root
    color: Style.background

    signal backClicked()
    property string pageTitle: ""
    property string rightButtonIcon: ""
    property string rightButtonObjectName: ""
    signal rightButtonClicked()
    default property alias contentData: contentArea.data

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(16)
        spacing: Style.fromPixel(20)

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.fromPixel(10)

            FontAwesomeButton {
                fontAwesomeIcon: "\uf053"
                iconColor: Style.iconColor
                iconSize: Style.fromPixel(18)
                backgroundColor: "transparent"
                onClicked: root.backClicked()
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: 1

                Text {
                    anchors.centerIn: parent
                    text: root.pageTitle
                    visible: root.pageTitle.length > 0
                    color: "white"
                    font.pixelSize: Style.fromPixel(18)
                    font.bold: true
                }
            }

            FontAwesomeButton {
                objectName: root.rightButtonObjectName
                visible: root.rightButtonIcon.length > 0
                fontAwesomeIcon: root.rightButtonIcon
                backgroundColor: "transparent"
                iconColor: "white"
                onClicked: root.rightButtonClicked()
            }
        }

        Item {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
