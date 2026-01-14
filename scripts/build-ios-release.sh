#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
trap 'echo "Build failed with exit code $?"; exit 1' ERR

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd ${SCRIPT_DIR}/..

rm -rf build-ios-release

./scripts/unlock-ios-keychain.sh

cmake --preset ios-release
cmake --build --preset ios-release

./scripts/archive-ios.sh build-ios-release
