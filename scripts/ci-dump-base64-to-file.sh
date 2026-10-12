#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

if [[ -z "$APPLE_CERTIFICATE_P12_BASE64" || -z "$APPLE_CERTIFICATE_P12_PATH" ]]; then
	echo "This script requires APPLE_CERTIFICATE_P12_BASE64 and APPLE_CERTIFICATE_P12_PATH to be set."
	exit 1
fi
if [[ -n "$APPLE_PROVISIONING_PROFILE_BASE64" && -z "$APPLE_PROVISIONING_PROFILE_PATH" ]]; then
	echo "APPLE_PROVISIONING_PROFILE_BASE64 is set, so APPLE_PROVISIONING_PROFILE_PATH must be set too."
	exit 1
fi

echo "$APPLE_CERTIFICATE_P12_BASE64" | base64 --decode >"$APPLE_CERTIFICATE_P12_PATH"
file "$APPLE_CERTIFICATE_P12_PATH"
echo "Decoded certificate written to $APPLE_CERTIFICATE_P12_PATH"

if [[ -n "$APPLE_PROVISIONING_PROFILE_BASE64" ]]; then
	echo "$APPLE_PROVISIONING_PROFILE_BASE64" | base64 --decode >"$APPLE_PROVISIONING_PROFILE_PATH"
	file "$APPLE_PROVISIONING_PROFILE_PATH"
	echo "Decoded provisioning profile written to $APPLE_PROVISIONING_PROFILE_PATH"
fi
