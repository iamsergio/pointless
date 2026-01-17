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

if ! gh auth status >/dev/null 2>&1; then
    echo "Not authenticated. Starting login..."
    gh auth login
else
    echo "Already authenticated."
fi

base64 -i "${APPLE_CERTIFICATE_P12_PATH}" | gh secret set APPLE_CERTIFICATE_P12_BASE64
base64 -i "${APPLE_PROVISIONING_PROFILE_PATH}" | gh secret set APPLE_PROVISIONING_PROFILE_BASE64

echo "Enter KEYCHAIN_PASSWORD:"
read -s KEYCHAIN_PASSWORD
echo -n "${KEYCHAIN_PASSWORD}" | gh secret set KEYCHAIN_PASSWORD

echo "Enter APPLE_P12_PASSWORD:"
read -s APPLE_P12_PASSWORD
echo -n "${APPLE_P12_PASSWORD}" | gh secret set APPLE_P12_PASSWORD
