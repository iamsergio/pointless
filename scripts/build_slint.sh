#!/bin/bash

# SPDX-FileCopyrightText: 2026 Sergio Martins
#
# SPDX-License-Identifier: MIT

# Builds Slint (3rdparty/slint) as a standalone C++ library, with the Qt backend enabled,
# and installs it into build-slint/install.
#
# The Qt backend makes Slint run on Qt's event loop, so the existing Qt code keeps working
# while the UI is ported to Slint.
#
# Consume it with: -DCMAKE_PREFIX_PATH=<repo>/build-slint/install ; find_package(Slint)
#
# Environment:
#   QT_DIR      Qt prefix to build the Qt backend against. Must be the same Qt the app uses.
#               Defaults to the Qt of the qmake6 in PATH.
#   BUILD_TYPE  Defaults to Release.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SLINT_SRC=${SCRIPT_DIR}/3rdparty/slint/api/cpp
BUILD_DIR=${SCRIPT_DIR}/build-slint
INSTALL_DIR=${BUILD_DIR}/install
BUILD_TYPE=${BUILD_TYPE:-Release}

if [ ! -f "${SLINT_SRC}/CMakeLists.txt" ]; then
    echo "Error: ${SLINT_SRC} not found. Run: git submodule update --init 3rdparty/slint"
    exit 1
fi

if ! command -v cargo >/dev/null; then
    echo "Error: cargo not found. Install Rust via rustup."
    exit 1
fi

if [ -z "$QT_DIR" ]; then
    if ! command -v qmake6 >/dev/null; then
        echo "Error: qmake6 not in PATH. Set QT_DIR to your Qt prefix."
        exit 1
    fi
    QT_DIR=$(qmake6 -query QT_INSTALL_PREFIX)
fi

if [ ! -f "${QT_DIR}/lib/cmake/Qt6Widgets/Qt6WidgetsConfig.cmake" ]; then
    echo "Error: Qt6Widgets not found in ${QT_DIR}"
    exit 1
fi

echo "Building Slint against Qt at ${QT_DIR}"

# EXPERIMENTAL, TESTING and SYSTEM_TESTING are needed by Spix's Slint backend (GUI tests).
cmake -S "${SLINT_SRC}" -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DCMAKE_PREFIX_PATH="${QT_DIR}" \
    -DBUILD_SHARED_LIBS=ON \
    -DSLINT_FEATURE_BACKEND_QT=ON \
    -DSLINT_FEATURE_EXPERIMENTAL=ON \
    -DSLINT_FEATURE_TESTING=ON \
    -DSLINT_FEATURE_SYSTEM_TESTING=ON \
    -DSLINT_FEATURE_INTERPRETER=OFF \
    -DSLINT_FEATURE_SYSTEM_TRAY=OFF \
    -DSLINT_FEATURE_GETTEXT=OFF

# Slint silently builds without the Qt backend if it can't find Qt, so check.
if ! grep -q "^Qt6Widgets_DIR:PATH=${QT_DIR}" "${BUILD_DIR}/CMakeCache.txt"; then
    echo "Error: Slint's CMake didn't pick up Qt6Widgets from ${QT_DIR}, Qt backend would be disabled"
    exit 1
fi

cmake --build "${BUILD_DIR}"
cmake --install "${BUILD_DIR}"

if ! ldd "${INSTALL_DIR}/lib/libslint_cpp.so" | grep -q "libQt6Widgets"; then
    echo "Error: libslint_cpp.so isn't linked against Qt6Widgets, Qt backend is missing"
    exit 1
fi

echo "Slint installed to ${INSTALL_DIR}"
