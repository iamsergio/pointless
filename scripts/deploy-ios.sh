#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../build-ios-dev/

xcrun devicectl device install app --device "$IPHONE_DEVICE_ID" ./bin/Release/pointless.app
