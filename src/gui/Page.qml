// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Layouts

import pointless 1.0

Rectangle {
    id: root
    color: Style.background
    visible: pageId.length > 0 && GuiController.currentPage === pageId

    property string pageId: ""
    signal backClicked()
    property string pageTitle: ""
    property string rightButtonIcon: ""
    property string rightButtonObjectName: ""
    signal rightButtonClicked()
    property string backButtonText: ""
    property string rightButtonText: ""
    default property alias contentData: contentArea.data
    property alias headerData: centerArea.data
    property alias overlayData: overlay.data

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.fromPixel(16)
        spacing: Style.fromPixel(20)

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: Style.fromPixel(50)

            IconButton {
                id: backButton
                objectName: "backButton"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                fontAwesomeIcon: "\uf053"
                textLabel: root.backButtonText
                iconColor: root.backButtonText.length > 0 ? Style.inkSecondary : Style.ink
                iconSize: Style.fromPixel(root.backButtonText.length > 0 ? 15 : 18)
                onClicked: root.backClicked()
            }

            Text {
                anchors.centerIn: parent
                width: Math.min(implicitWidth, parent.width - 2 * (backButton.width + Style.fromPixel(10)))
                text: root.pageTitle
                visible: root.pageTitle.length > 0
                color: Style.ink
                font.pixelSize: Style.fromPixel(16)
                font.weight: Font.DemiBold
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                id: centerArea
                anchors.left: backButton.right
                anchors.leftMargin: Style.fromPixel(10)
                anchors.right: rightButton.visible ? rightButton.left : parent.right
                anchors.rightMargin: rightButton.visible ? Style.fromPixel(10) : 0
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }

            IconButton {
                id: rightButton
                objectName: root.rightButtonObjectName
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: root.rightButtonIcon.length > 0 || root.rightButtonText.length > 0
                fontAwesomeIcon: root.rightButtonIcon
                textLabel: root.rightButtonText
                diameter: Style.fromPixel(root.rightButtonText.length > 0 ? 34 : 40)
                fillColor: root.rightButtonText.length > 0 ? Style.accent : "transparent"
                iconColor: root.rightButtonText.length > 0 ? Style.accentInk : Style.ink
                iconSize: Style.fromPixel(root.rightButtonText.length > 0 ? 14 : 18)
                onClicked: root.rightButtonClicked()
            }
        }

        Item {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Item {
        id: overlay
        anchors.fill: parent
        z: 10
    }
}
