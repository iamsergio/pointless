#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

mkdir -p ~/keychain/

if [ -z "${KEYCHAIN_FILE}" ]; then
	echo "KEYCHAIN_FILE environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_CERTIFICATE_P12_PATH}" ]; then
	echo "APPLE_CERTIFICATE_P12_PATH environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_PROVISIONING_PROFILE_PATH}" ]; then
	echo "APPLE_PROVISIONING_PROFILE_PATH environment variable must be set"
	exit 1
fi

if [ ! -f "${APPLE_PROVISIONING_PROFILE_PATH}" ]; then
	echo "Provisioning profile not found at ${APPLE_PROVISIONING_PROFILE_PATH}"
	exit 1
fi

if [ ! -f "${APPLE_CERTIFICATE_P12_PATH}" ]; then
	echo "Certificate .p12 file not found at ${APPLE_CERTIFICATE_P12_PATH}"
	exit 1
fi

if [ -z "${KEYCHAIN_PASSWORD}" ]; then
	echo "KEYCHAIN_PASSWORD environment variable not set, please enter password:"
	read -rs KEYCHAIN_PASSWORD
fi

if [ -z "${APPLE_P12_PASSWORD}" ]; then
	echo "APPLE_P12_PASSWORD environment variable not set, please enter password:"
	read -rs APPLE_P12_PASSWORD
fi

mkdir -p "$(dirname "${KEYCHAIN_FILE}")"

if [ ! -f "${KEYCHAIN_FILE}" ]; then
	security create-keychain -p "${KEYCHAIN_PASSWORD}" "${KEYCHAIN_FILE}"
	echo "Created new keychain at ${KEYCHAIN_FILE}"
else
	echo "Keychain already exists at ${KEYCHAIN_FILE}"
fi

security unlock-keychain -p "${KEYCHAIN_PASSWORD}" "${KEYCHAIN_FILE}"
security set-keychain-settings -u "${KEYCHAIN_FILE}"

security list-keychains -s \
	"${KEYCHAIN_FILE}" \
	"${HOME}/Library/Keychains/login.keychain-db" \
	"/Library/Keychains/System.keychain"

echo "---"
echo "New keychain search list:"
# To reset them: ./restore-ios-keychain.sh;
security list-keychains -d user
echo "---"

# install provisioning profile (in new and old locations)
echo "Installing provisioning profile..."
mkdir -p ~/Library/Developer/Xcode/UserData/Provisioning\ Profiles
mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
cp "${APPLE_PROVISIONING_PROFILE_PATH}" ~/Library/Developer/Xcode/UserData/Provisioning\ Profiles/
cp "${APPLE_PROVISIONING_PROFILE_PATH}" ~/Library/MobileDevice/Provisioning\ Profiles

# import certificate
echo "Importing Apple certificate into keychain..."
security import "${APPLE_CERTIFICATE_P12_PATH}" -k "${KEYCHAIN_FILE}" -P "$APPLE_P12_PASSWORD" -A

# download and install Apple WWDR certificate
echo "Downloading and installing Apple WWDR certificate..."
curl -fLo AppleWWDRCA.cer https://developer.apple.com/certificationauthority/AppleWWDRCA.cer
security import AppleWWDRCA.cer -k "${KEYCHAIN_FILE}" -T /usr/bin/codesign
rm AppleWWDRCA.cer

# allow codesign to access the key
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k "$KEYCHAIN_PASSWORD" "${KEYCHAIN_FILE}"
