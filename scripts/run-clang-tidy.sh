#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${1:-${BUILD_DIR:-$root/build-dev}}"
clang_tidy="${CLANG_TIDY:-clang-tidy}"
jobs="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu)}"

if [[ ! -f "$build_dir/compile_commands.json" ]]; then
  echo "error: $build_dir/compile_commands.json not found; configure with CMAKE_EXPORT_COMPILE_COMMANDS=ON" >&2
  exit 2
fi

cd "$root"
git ls-files -z -- \
    '*.cpp' '*.h' '*.c' '*.cc' '*.cxx' '*.hh' '*.hxx' \
    ':!:3rdparty' ':!:slint' ':!:**/tests/**' |
  xargs -0 -n 1 -P "$jobs" sh -c 'echo "$2"; exec "$0" -p "$1" --quiet --warnings-as-errors="*" "$2"' \
    "$clang_tidy" "$build_dir" 2>&1 |
  sed -u -E '/^[0-9]+ warnings? generated\.$/d'
