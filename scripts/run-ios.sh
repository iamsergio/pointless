#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

# Runs app and gives you a console

xcrun devicectl device process launch --device "${IPHONE_DEVICE_ID}" --console com.iamsergio.pointless
