#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

cd ~/Library/Developer/Xcode/UserData/Provisioning\ Profiles

for f in *.mobileprovision; do
	echo "File: $f"
	security cms -D -i "$f" | grep -aA1 "Name\|AppIDName\|UUID" | grep "<string>"
	echo "-----------------------------------"
done
