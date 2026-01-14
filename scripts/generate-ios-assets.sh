#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

APPICON_DIR="${SCRIPT_DIR}/../assets/ios/Images.xcassets/AppIcon.appiconset"
SOURCE_IMAGE="${SCRIPT_DIR}/../assets/icon.png"

mkdir -p "$APPICON_DIR"

# 1. Generate the specific 120x120 file and others
# iPhone App @2x (120x120) and @3x (180x180)
convert "$SOURCE_IMAGE" -resize 120x120 -strip "$APPICON_DIR/icon_120.png"
convert "$SOURCE_IMAGE" -resize 180x180 -strip "$APPICON_DIR/icon_180.png"
# Add a few more basics to be safe
convert "$SOURCE_IMAGE" -resize 40x40   -strip "$APPICON_DIR/icon_40.png"
convert "$SOURCE_IMAGE" -resize 60x60   -strip "$APPICON_DIR/icon_60.png"

convert "$SOURCE_IMAGE" -resize 1024x1024 -strip "$APPICON_DIR/icon_1024.png"

# Create Contents.json
cat <<EOF > "$APPICON_DIR/Contents.json"
{
  "images" : [
    { "size" : "20x20", "idiom" : "iphone", "filename" : "icon_40.png", "scale" : "2x" },
    { "size" : "20x20", "idiom" : "iphone", "filename" : "icon_60.png", "scale" : "3x" },
    { "size" : "29x29", "idiom" : "iphone", "filename" : "icon_58.png", "scale" : "2x" },
    { "size" : "40x40", "idiom" : "iphone", "filename" : "icon_80.png", "scale" : "2x" },
    { "size" : "40x40", "idiom" : "iphone", "filename" : "icon_120.png", "scale" : "3x" },
    { "size" : "60x60", "idiom" : "iphone", "filename" : "icon_120.png", "scale" : "2x" },
    { "size" : "60x60", "idiom" : "iphone", "filename" : "icon_180.png", "scale" : "3x" },
    { "size" : "1024x1024", "idiom" : "ios-marketing", "filename" : "icon_1024.png", "scale" : "1x" }
  ],
  "info" : { "version" : 1, "author" : "xcode" }
}
EOF
