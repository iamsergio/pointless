#!/bin/bash

set -e

unset LD_LIBRARY_PATH

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if ! rustup target list --installed | grep -q "armv7-unknown-linux-gnueabihf"; then
    echo "Error: armv7-unknown-linux-gnueabihf target not installed. Install it with: rustup target add armv7-unknown-linux-gnueabihf"
    exit 1
fi

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

export PKG_CONFIG_ALLOW_CROSS=1
export PKG_CONFIG_SYSROOT_DIR=$SDKTARGETSYSROOT

export TARGET="armv7-unknown-linux-gnueabihf"

# CC/CXX env vars from the SDK contain embedded flags (e.g. "-mthumb -mfpu=neon ...").
# Cargo/rustc split RUSTFLAGS and CARGO_TARGET_*_LINKER on whitespace, so we must
# wrap the compiler in a single-path wrapper script to avoid misinterpretation.
CC_BIN=$(echo "$CC" | awk '{print $1}')
CXX_BIN=$(echo "$CXX" | awk '{print $1}')
CC_EXTRA=$(echo "$CC" | cut -d' ' -f2-)
CXX_EXTRA=$(echo "$CXX" | cut -d' ' -f2-)

# Create a linker wrapper so cargo can reference it as a single path
LINKER_WRAPPER=$(mktemp /tmp/armv7-linker-XXXXXX.sh)
cat > "$LINKER_WRAPPER" << EOF
#!/bin/bash
exec "$CXX_BIN" $CXX_EXTRA "\$@"
EOF
chmod +x "$LINKER_WRAPPER"
trap "rm -f '$LINKER_WRAPPER'" EXIT

# Use the wrapper as the linker (single path, no embedded spaces)
export CARGO_TARGET_ARMV7_UNKNOWN_LINUX_GNUEABIHF_LINKER="$LINKER_WRAPPER"

export CC_armv7_unknown_linux_gnueabihf="$CC_BIN"
export CXX_armv7_unknown_linux_gnueabihf="$CXX_BIN"
export CFLAGS_armv7_unknown_linux_gnueabihf="$CC_EXTRA"
export CXXFLAGS_armv7_unknown_linux_gnueabihf="$CXX_EXTRA"

# No RUSTFLAGS linker override needed — CARGO_TARGET_*_LINKER handles it
unset RUSTFLAGS

echo "Using Rust target: armv7-unknown-linux-gnueabihf with linker: $CARGO_TARGET_ARMV7_UNKNOWN_LINUX_GNUEABIHF_LINKER"
echo "Using PKG_CONFIG_SYSROOT_DIR: $PKG_CONFIG_SYSROOT_DIR"

cd ${SCRIPT_DIR}/../slint/

export SLINT_NO_QT=1

cargo build --release --target=armv7-unknown-linux-gnueabihf --features slint/backend-winit,slint/renderer-femtovg
