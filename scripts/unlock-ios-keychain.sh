#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

if ! security show-keychain-info "${KEYCHAIN_FILE}" 2>&1 | grep -q "User interaction is not allowed"; then
	echo "Keychain is already unlocked."
	exit 0
fi

if [ -z "${KEYCHAIN_FILE}" ]; then
	echo "KEYCHAIN_FILE environment variable must be set"
	exit 1
fi

if [ -z "${KEYCHAIN_PASSWORD}" ]; then
	echo "KEYCHAIN_PASSWORD environment variable not set, please enter password:"
	read -rs KEYCHAIN_PASSWORD
fi

security unlock-keychain -p "${KEYCHAIN_PASSWORD}" "${KEYCHAIN_FILE}"
