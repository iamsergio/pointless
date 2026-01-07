#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BIN_DIR="Release"
while [[ $# -gt 0 ]]; do
	case "$1" in
	--debug)
		BIN_DIR="Debug"
		shift
		;;
	*)
		echo "Unknown argument: $1" >&2
		echo "Usage: $0 [--debug]" >&2
		exit 1
		;;
	esac
done

if [[ -z "${IPHONE_DEVICE_ID:-}" ]]; then
	echo "IPHONE_DEVICE_ID is not set" >&2
	exit 1
fi

cd "${SCRIPT_DIR}"/../build-ios-dev/

APP_PATH="$(pwd)/bin/${BIN_DIR}/pointless.app"
if [[ ! -d "$APP_PATH" ]]; then
	echo "Error: Application bundle not found at $APP_PATH" >&2
	echo "Pass --debug for a debug build." >&2
	exit 1
fi

echo "Installing $APP_PATH ..."
xcrun devicectl device install app --device "$IPHONE_DEVICE_ID" "$APP_PATH"
