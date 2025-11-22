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

    property color background: "#2d2e32"
    readonly property color background2: "#141b28"
    property color buttonBackground: "#414244"
    readonly property color buttonActive: "#0290f6"
    readonly property color sectionBackgroundColor: "#1a2332"
    readonly property color sectionTextColor: "#ffffff"
    readonly property color taskTextColor: "#e0e0e0"
    readonly property color taskSecondaryTextColor: "#808080"
    readonly property color taskCompletedTextColor: "#606060"
    readonly property color taskBackground: "#3e3f43"

    readonly property int sectionFontSize: fromPixel(14)

    readonly property color textColor2: "#98999e"
    readonly property color iconColor: textColor2
    readonly property int taskSpacing: fromPixel(12)

    // week-view:
    readonly property int weekdayFontSize: fromPixel(16)
    readonly property color weekdayTextColor: "#ffffff"
}
