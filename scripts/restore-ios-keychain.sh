#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

echo "---"
echo "Old keychain search list:"
security list-keychains

security list-keychains -s \
	"${HOME}/Library/Keychains/login.keychain-db" \
	"/Library/Keychains/System.keychain"

echo "---"
echo "New keychain search list:"
security list-keychains
