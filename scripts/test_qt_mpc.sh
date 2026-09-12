#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

set -uo pipefail

# Manual repro for a qt-commander injection crash: attaching and injecting
# libqt-commander.so into a running `pointless` reliably kills the target
# with "qt_safe_poll (Invalid argument)" shortly after injection completes.
# Bypasses the Python MCP server entirely -- drives qt-injector directly,
# the same way qt_commander/rpc_client.py::inject_and_connect() does.
#
# Usage: scripts/test_qt_mpc.sh <build-dir>   (e.g. build-dev, build-dev-asan)
#
# <build-dir> must already be configured with -DPOINTLESS_BUILD_QT_COMMANDER=ON
# and built (this script never builds anything), plus the usual POINTLESS_*
# env vars (see top-level CLAUDE.md).

if [[ $# -ne 1 ]]; then
    echo "usage: $0 <build-dir>   (e.g. build-dev, build-dev-asan)" >&2
    exit 1
fi
build_dir="$1"

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

pointless_bin="$build_dir/bin/pointless"
injector_bin=".qt-commander/bin/qt-injector"
lib_path=".qt-commander/bin/libqt-commander.so"

for f in "$pointless_bin" "$injector_bin" "$lib_path"; do
    if [[ ! -e "$f" ]]; then
        echo "missing: $f -- configure '$build_dir' with -DPOINTLESS_BUILD_QT_COMMANDER=ON and build it first" >&2
        exit 1
    fi
done

session_dir=".qt-commander/sessions/repro-crash"
rm -rf "$session_dir"
mkdir -p "$session_dir"
port_file="$session_dir/port.txt"

# Assumes core_pattern is already configured (host-wide) to write core files
# under /tmp -- this script doesn't touch core_pattern itself.
ulimit -c unlimited

core_marker="$(mktemp /tmp/pointless-repro-marker-XXXXXX)"
log_file="$(mktemp /tmp/pointless-repro-XXXXXX.log)"
echo "== launching $pointless_bin (log: $log_file) =="
"$pointless_bin" >"$log_file" 2>&1 &
pid=$!

# Give the QML engine time to finish loading before attaching.
sleep 3

if ! kill -0 "$pid" 2>/dev/null; then
    echo "pointless (pid $pid) exited before injection was attempted; see $log_file" >&2
    exit 1
fi

echo "== pointless pid=$pid, attaching with $injector_bin =="
timeout 30 "$injector_bin" "$pid" "$lib_path" "$port_file"
injector_status=$?

sleep 1

echo
if kill -0 "$pid" 2>/dev/null; then
    echo "RESULT: pointless (pid $pid) is still alive after injection (injector exit=$injector_status) -- crash NOT reproduced."
    kill "$pid" 2>/dev/null
    exit_code=1
else
    echo "RESULT: pointless (pid $pid) crashed during/after injection (injector exit=$injector_status), as expected."
    echo "---- tail of $log_file (includes the ASan report, if this is an ASan build) ----"
    tail -n 60 "$log_file"

    core_file=""
    for _ in $(seq 1 20); do
        core_file="$(find /tmp -maxdepth 1 -newer "$core_marker" -name "*$pid*" 2>/dev/null | head -n1)"
        [[ -n "$core_file" ]] && break
        sleep 0.5
    done

    if [[ -n "$core_file" ]] && command -v gdb >/dev/null; then
        echo
        echo "---- backtrace ($core_file) ----"
        gdb -batch -ex "thread apply all bt" -ex quit "$pointless_bin" "$core_file" 2>&1
    elif [[ -z "$core_file" ]]; then
        echo "no core file found under /tmp for pid $pid -- check core_pattern/ulimit -c" >&2
    else
        echo "gdb not found -- install it to get a backtrace from $core_file" >&2
    fi
    exit_code=0
fi

rm -rf "$session_dir"
rm -f "$core_marker"
exit "$exit_code"
