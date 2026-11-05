// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pragma Singleton

import QtQuick 2.15

import pointless 1.0

Item {
    id: root
    readonly property double scaleFactor: 1.0
    readonly property bool isMobile: GuiController.isMobile

    function fromPixel(px) {
        if (root.isMobile || GuiController.isYocto)
            return px;

        return px * scaleFactor;
    }

    property color background: "#11141B"
    readonly property color surface: "#1A1E27"
    readonly property color raised: "#242935"
    readonly property color line: "#2B313E"
    readonly property color ink: "#ECEFF5"
    readonly property color inkSecondary: "#A3ABBC"
    readonly property color inkTertiary: "#6F778A"
    readonly property color accent: "#6F8CFF"
    readonly property color accentInk: "#0B1230"
    readonly property color accentSoft: "#1F2A4D"
    readonly property color accentText: "#A3B5FF"
    readonly property color amber: "#F0B44C"
    readonly property color dusk: "#A897F6"
    readonly property color duskSoft: "#272241"
    readonly property color good: "#3CCB8A"
    readonly property color danger: "#FF6E61"
    readonly property color dangerSoft: "#3B1F1E"
}
