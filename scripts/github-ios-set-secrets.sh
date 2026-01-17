#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

# Copy the 2 outputs into GH secrets with name APPLE_CERTIFICATE_P12_BASE64 and APPLE_PROVISIONING_PROFILE_BASE64

set -e

if [ -z "${APPLE_CERTIFICATE_P12_PATH}" ]; then
	echo "APPLE_CERTIFICATE_P12_PATH environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_PROVISIONING_PROFILE_PATH}" ]; then
	echo "APPLE_PROVISIONING_PROFILE_PATH environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_PROVISIONING_PROFILE_UUID}" ]; then
	echo "APPLE_PROVISIONING_PROFILE_UUID environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_TEAM_ID}" ]; then
	echo "APPLE_TEAM_ID environment variable must be set"
	exit 1
fi

if [ -z "${APPLE_SPECIFIC_PASSWORD}" ]; then
	echo "APPLE_SPECIFIC_PASSWORD environment variable must be set"
	exit 1
fi

if [ -z "${POINTLESS_APPLE_ID}" ]; then
	echo "POINTLESS_APPLE_ID environment variable must be set"
	exit 1
fi

if [ -z "${POINTLESS_APP_APPLE_ID}" ]; then
	echo "POINTLESS_APP_APPLE_ID environment variable must be set"
	exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
	echo "Not authenticated. Starting login..."
	gh auth login
else
	echo "Already authenticated."
fi

base64 -i "${APPLE_CERTIFICATE_P12_PATH}" | gh secret set APPLE_CERTIFICATE_P12_BASE64
base64 -i "${APPLE_PROVISIONING_PROFILE_PATH}" | gh secret set APPLE_PROVISIONING_PROFILE_BASE64

KEYCHAIN_PASSWORD=$(openssl rand -base64 16) # not really important, just a random password
echo -n "${KEYCHAIN_PASSWORD}" | gh secret set KEYCHAIN_PASSWORD

echo "Enter APPLE_P12_PASSWORD:"
read -rs APPLE_P12_PASSWORD
echo -n "${APPLE_P12_PASSWORD}" | gh secret set APPLE_P12_PASSWORD

echo -n "${APPLE_PROVISIONING_PROFILE_UUID}" | gh secret set APPLE_PROVISIONING_PROFILE_UUID
echo -n "${APPLE_TEAM_ID}" | gh secret set APPLE_TEAM_ID
echo -n "${APPLE_SPECIFIC_PASSWORD}" | gh secret set APPLE_SPECIFIC_PASSWORD
echo -n "${POINTLESS_APPLE_ID}" | gh secret set POINTLESS_APPLE_ID
echo -n "${POINTLESS_APP_APPLE_ID}" | gh secret set POINTLESS_APP_APPLE_ID
