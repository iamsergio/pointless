// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import QtQuick 2.15
import QtQuick.Controls 2.15

FontAwesomeButton {
    id: root

    width: 28
    height: 28
    fontAwesomeIcon: root.checked ? "\uf14a" : "\uf0c8"
    iconSize: 20
    iconColor: root.checked ? "#135bec" : "#135bec"
    onClicked: root.checked = !root.checked
}
