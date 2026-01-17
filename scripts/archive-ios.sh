#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

# Prerequisites:
# - Be sure to build 1st with Release configuration
# - for example: cmake --preset ios-release && cmake --build --preset ios-release

# 1. Generates pointless.xcarchive inside the build directory
# 2. Generates the .ipa file for TestFlight distribution

if [ -z "$1" ]; then
	echo "Usage: $0 <build-directory>"
	exit 1
fi

build_dir="$1"

if [ ! -d "$build_dir" ]; then
	echo "Error: Build directory '$build_dir' does not exist"
	exit 1
fi

# back to src root
script_dir=$(dirname "$(realpath "$0")")
cd "$script_dir"/..

cd "$build_dir"

xcodebuild -scheme pointless -configuration Release -destination "generic/platform=iOS" -archivePath pointless.xcarchive archive

xcodebuild -exportArchive -archivePath pointless.xcarchive -exportOptionsPlist ./ExportOptions-TestFlight.plist -exportPath pointless_ipa -allowProvisioningUpdates
