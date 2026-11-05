#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../

PRESET=rel
DEST=${PUB_DATA_DIR}/installed/pointless/darwin

cmake --preset=${PRESET}
cmake --build build-${PRESET}
mkdir -p "${DEST}"
rm -rf "${DEST}/pointless.app"
cp -R build-${PRESET}/bin/pointless.app "${DEST}"

echo
echo "Deployed pointless.app to ${DEST}"
