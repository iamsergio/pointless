// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma Singleton

import QtQuick 2.15

Item {

    readonly property double scaleFactor: 1.5

    function fromPixel(px) {
        if (Controller.isMobile)
            return px;

        return px * scaleFactor;
    }

    property color background: "#101622"
    readonly property color background2: "#141b28"
    property color buttonBackground: "#242e3c"
    readonly property color buttonActive: "#135bec"
    readonly property color sectionBackgroundColor: "#1a2332"
    readonly property color sectionTextColor: "#ffffff"
    readonly property color taskTextColor: "#e0e0e0"
    readonly property color taskSecondaryTextColor: "#808080"
    readonly property color taskCompletedTextColor: "#606060"
    readonly property int sectionFontSize: fromPixel(14)

    // week-view:
    readonly property int weekdayFontSize: fromPixel(20)
    readonly property color weekdayTextColor: "#ffffff"
}
