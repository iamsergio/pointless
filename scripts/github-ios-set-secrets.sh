#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

# Copy the 2 outputs into GH secrets with name APPLE_CERTIFICATE_P12 and APPLE_PROVISIONING_PROFILE

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

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
