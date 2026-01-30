#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../

PRESET=rel
DEST=${PUB_DATA_DIR}/etc/bin/

cmake --preset=${PRESET}
cmake --build build-${PRESET}
cp build-${PRESET}/bin/pointless "${DEST}"

echo
echo "Deployed pointless to ${DEST}"
