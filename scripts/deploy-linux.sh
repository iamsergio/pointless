#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../

PRESET=rel
DEST=/pub_data/installed/pointless/linux

cmake --preset=${PRESET}
cmake --build build-${PRESET}
mkdir -p "${DEST}"
cp build-${PRESET}/bin/pointless "${DEST}"
cp assets/icon.png "${DEST}/pointless.png"

# The filename must match the app_id set via QGuiApplication::setDesktopFileName()
DESKTOP_FILE="${DEST}/share/applications/com.iamsergio.pointless.desktop"
mkdir -p "$(dirname "${DESKTOP_FILE}")"
cat > "${DESKTOP_FILE}" <<EOF
[Desktop Entry]
Type=Application
Name=Pointless
Exec=${DEST}/pointless
Icon=${DEST}/pointless.png
Categories=Office;ProjectManagement;
StartupWMClass=pointless
EOF

echo
echo "Deployed pointless to ${DEST}"
echo "Installed ${DESKTOP_FILE}"
