// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma Singleton

import QtQuick 2.15

import pointless 1.0

Item {
    id: root
    readonly property double scaleFactor: 1.5
    readonly property bool isMobile: GuiController.isMobile

    function fromPixel(px) {
        if (root.isMobile)
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
    readonly property color taskCalendarTextDisabled: "#405040"
    readonly property color taskBackground: "#3e3f43"
    readonly property color taskImportantBackground: "#f07e01"
    readonly property color taskEveningBackground: "#484a65"
    readonly property int taskHeight: fromPixel(55)
    readonly property int taskEveningHeight: fromPixel(29)

    readonly property int sectionFontSize: fromPixel(14)

    readonly property color textColor2: "#98999e"
    readonly property color iconColor: textColor2
    readonly property int taskSpacing: fromPixel(12)

    // week-view:
    readonly property int weekdayFontSize: fromPixel(16)
    readonly property color weekdayTextColor: "#ffffff"

    // calendar:
    readonly property color calendarBackground: "#222327"
    readonly property color calendarHighlight: "#0290f6"
    readonly property color calendarText: "#ffffff"
    readonly property color calendarDisabledText: "#405040"
    readonly property color calendarSecondaryText: "#98999e"

    // plus button:
    readonly property color plusButtonColor: "#0ea5e9"
    readonly property int plusButtonSize: GuiController.isMobile ? fromPixel(32) : fromPixel(32)

    // menu:
    readonly property int menuHeight: fromPixel(35)

    // buttons:
    readonly property color buttonColor: "#0ea5e9"
    readonly property color buttonTextColor: "#000000"
}
