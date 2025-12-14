#!/bin/bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -e

trap 'echo "ERROR: pre-push checks failed!"' ERR

pre-commit run --all-files

cmake --preset=dev
cmake --build build-dev
cd build-dev
ninja all_qmllint
ctest -j2 --output-on-failure --verbose
cd ..
cmake --preset=ci-clang-tidy
cmake --build build-ci-clang-tidy
cmake --preset=clazy
cmake --build build-clazy
