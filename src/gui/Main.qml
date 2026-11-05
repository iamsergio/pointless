// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick
import QtQuick.Controls.Fusion as QC

import pointless 1.0

QC.ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"

    visible: true
    width: Style.fromPixel(640)
    height: Style.fromPixel(800)
    visibility: (GuiController.isMobile || GuiController.isEGLFS) ? QC.ApplicationWindow.FullScreen : QC.ApplicationWindow.Windowed
    flags: Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint

    title: GuiController.windowTitle
    topPadding: 0
    bottomPadding: 0

    MainView {
        anchors.fill: parent
    }
}
