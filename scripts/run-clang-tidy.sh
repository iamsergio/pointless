#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${1:-${BUILD_DIR:-$root/build-dev}}"
clang_tidy="${CLANG_TIDY:-clang-tidy}"
run_clang_tidy="${RUN_CLANG_TIDY:-run-clang-tidy}"
jobs="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu)}"

if [[ ! -f "$build_dir/compile_commands.json" ]]; then
  echo "error: $build_dir/compile_commands.json not found; configure with CMAKE_EXPORT_COMPILE_COMMANDS=ON" >&2
  exit 2
fi

"$run_clang_tidy" -p "$build_dir" -j "$jobs" -clang-tidy-binary "$clang_tidy" \
  -warnings-as-errors='*' "^$root/src/(?!.*/tests/)" 2>&1 |
  sed -u -E '/^[0-9]+ warnings? generated\.$/d'
