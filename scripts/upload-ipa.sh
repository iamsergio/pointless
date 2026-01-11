#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

if [ -z "$1" ]; then
	echo "Usage: $0 <build-directory>"
	exit 1
fi

build_dir="$1"

script_dir=$(dirname "$(realpath "$0")")
cd "$script_dir"/..
cd "$build_dir"
if [ ! -d "pointless_ipa" ]; then
    echo "Error: pointless_ipa directory does not exist. Did you run archive-ios.sh first?"
    exit 1
fi

if [ ! -f "pointless_ipa/pointless.ipa" ]; then
    echo "Error: pointless_ipa/pointless.ipa does not exist. Did you run archive-ios.sh first?"
    exit 1
fi

if [ -z "$APPLE_SPECIFIC_PASSWORD" ]; then
    echo "Error: APPLE_SPECIFIC_PASSWORD environment variable is not set"
    exit 1
fi

if [ -z "$POINTLESS_APPLE_ID" ]; then
    echo "Error: POINTLESS_APPLE_ID environment variable is not set"
    exit 1
fi

if [ -z "$POINTLESS_APP_APPLE_ID" ]; then
    echo "Error: POINTLESS_APP_APPLE_ID environment variable is not set"
    exit 1
fi

# POINTLESS_APPLE_ID is the Apple ID email used to upload the app
# POINTLESS_APP_APPLE_ID is the app's Apple ID (the numeric one)

xcrun altool --upload-app -f pointless_ipa/pointless.ipa --type ios --username $POINTLESS_APPLE_ID \
    --password $APPLE_SPECIFIC_PASSWORD --apple-id $POINTLESS_APP_APPLE_ID
