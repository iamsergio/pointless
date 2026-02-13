#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ $# -lt 1 ]; then
	echo "Usage: $0 <build_dir>"
	exit 1
fi

BUILD_DIR="$1"
if [ ! -d "$BUILD_DIR/bin/pointless.app" ]; then
	echo "Error: $BUILD_DIR/bin/pointless.app not found"
	exit 1
fi

if [ -z "$APPLE_TEAM_ID" ]; then
	echo "Error: APPLE_TEAM_ID environment variable not set"
	exit 1
fi

# "Developer Id Application: <name> (<team_id>)"
if [ -z "$APPLE_DEV_ID_APPLICATION" ]; then
	echo "Error: APPLE_DEV_ID_APPLICATION environment variable not set"
	exit 1
fi

security unlock

echo "Running macdeployqt ..."
macdeployqt "$BUILD_DIR/bin/pointless.app" -qmldir=${SCRIPT_DIR}/..

echo "Signing frameworks..."
find $BUILD_DIR/bin/pointless.app/Contents/Frameworks -name "*.dylib" -or -name "*.framework" -exec codesign --force --verify --verbose \
	--sign ${APPLE_DEV_ID_APPLICATION} --options runtime --timestamp {} \;

echo "Signing plugins..."
find $BUILD_DIR/bin/pointless.app/Contents/PlugIns -name "*.dylib" -exec codesign --force --verify --verbose \
	--sign ${APPLE_DEV_ID_APPLICATION} --options runtime --timestamp {} \;

echo "Signing inner binary..."
codesign --force --timestamp --options runtime \
	--entitlements ${SCRIPT_DIR}/../src/mac/pointless.entitlements \
	--sign ${APPLE_DEV_ID_APPLICATION} \
	"$BUILD_DIR/bin/pointless.app/Contents/MacOS/pointless"

echo "Signing app bundle..."
codesign --force --timestamp --options runtime --sign ${APPLE_DEV_ID_APPLICATION} --entitlements ${SCRIPT_DIR}/../src/mac/pointless.entitlements "$BUILD_DIR/bin/pointless.app"
if ! codesign --verify --deep --verbose "$BUILD_DIR/bin/pointless.app"; then
	echo "Error: Code signing verification failed"
	exit 1
fi
