#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../

cmake --preset=rel
cmake --build build-rel
cp build-rel/bin/pointless /pub_data/installation/bin

echo
echo "Deployed pointless to /pub_data/installation/bin"
