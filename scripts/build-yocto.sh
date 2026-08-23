#!/bin/bash

# SPDX-FileCopyrightText: 2026 Sergio Martins
#
# SPDX-License-Identifier: MIT


set -e

unset LD_LIBRARY_PATH

if [ -n "$RPI2_SDK" ]; then
	SDK_FILE="$RPI2_SDK/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi"
else
	SDK_FILE=~/raspberrypi2_sdk/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
fi

if [ ! -f "$SDK_FILE" ]; then
	echo "Error: SDK file not found at $SDK_FILE"
	exit 1
fi

source "$SDK_FILE"

cmake --preset=yocto-rel
cmake --build build-yocto-rel
