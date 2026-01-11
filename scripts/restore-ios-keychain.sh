#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

security list-keychains -s \
    "${HOME}/Library/Keychains/login.keychain-db" "/Library/Keychains/System.keychain"
