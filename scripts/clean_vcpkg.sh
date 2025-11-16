#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}"/../3rdparty/vcpkg/
git clean -fdx

if [ -d "$HOME/.cache/vcpkg/archives" ]; then
	echo "Removing vcpkg archives cache..."
	rm -rf "$HOME/.cache/vcpkg/archives"
fi
