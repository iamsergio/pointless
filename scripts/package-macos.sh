#!/bin/bash

set -e

ARCH="arm64"
LIPO="x86_64"

if [ $# -ne 1 ]; then
    echo "Usage: $0 <build_dir>"
    exit 1
fi

if [ -z "$APPLE_TEAM_ID" ] || [ -z "$APPLE_ID" ] || [ -z "$APPLE_SPECIFIC_PASSWORD" ] || [ -z "$APPLE_DEV_ID_INSTALLER" ]; then
    echo "Error: Required environment variables not set (APPLE_TEAM_ID, APPLE_ID, APPLE_SPECIFIC_PASSWORD, APPLE_DEV_ID_INSTALLER)"
    exit 1
fi

BUILD_DIR=$1

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${BUILD_DIR}"

${SCRIPT_DIR}/sign-macos.sh

VERSION=$(cat version.txt)

pkgbuild --root Pointless.app \
     --identifier com.iamsergio.pointless \
     --version ${VERSION} \
     --install-location /Applications/Pointless.app \
     Pointless-unsigned.pkg #&& \
# productsign --sign $APPLE_DEV_ID_INSTALLER Pointless-unsigned.pkg Pointless.pkg && \
# xcrun notarytool submit  Pointless.pkg --apple-id ${APPLE_ID} --team-id $APPLE_TEAM_ID --password $APPLE_SPECIFIC_PASSWORD --wait && \
# xcrun stapler staple -v Pointless.pkg && \
# cd .. && \

