#!/usr/bin/env bash
# Offline Release Packaging Script
# Bundles binaries, extracted assets, launcher, and documentation into offline distribution archives.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

PLATFORM="${1:-native}"
OUTPUT_DIR="dist/$PLATFORM"

echo "Packaging release for target: $PLATFORM"
mkdir -p "$OUTPUT_DIR"

# Copy common files
cp README.md LICENSE launcher.py "$OUTPUT_DIR/"
cp -r tools "$OUTPUT_DIR/"
cp -r docs "$OUTPUT_DIR/"

if [ -d "assets" ]; then
    mkdir -p "$OUTPUT_DIR/assets"
    cp -r assets/* "$OUTPUT_DIR/assets/"
fi

# Copy binary if available
case "$PLATFORM" in
    win32|win10)
        [ -f "build/win32/cadog.exe" ] && cp "build/win32/cadog.exe" "$OUTPUT_DIR/"
        ;;
    win64|win11)
        [ -f "build/win64/cadog.exe" ] && cp "build/win64/cadog.exe" "$OUTPUT_DIR/"
        ;;
    macos)
        [ -d "build/macos/cadog.app" ] && cp -r "build/macos/cadog.app" "$OUTPUT_DIR/"
        ;;
    vita)
        [ -f "build/vita/cadog.vpk" ] && cp "build/vita/cadog.vpk" "$OUTPUT_DIR/"
        ;;
    3ds)
        [ -f "build/3ds/cadog.3dsx" ] && cp "build/3ds/cadog.3dsx" "$OUTPUT_DIR/"
        ;;
    wii)
        [ -f "build/wii/cadog.dol" ] && cp "build/wii/cadog.dol" "$OUTPUT_DIR/"
        ;;
    ps3)
        [ -f "build/ps3/EBOOT.BIN" ] && cp "build/ps3/EBOOT.BIN" "$OUTPUT_DIR/"
        ;;
    xbox360)
        [ -f "build/xbox360/cadog.xex" ] && cp "build/xbox360/cadog.xex" "$OUTPUT_DIR/"
        ;;
    *)
        [ -f "build/linux/cadog" ] && cp "build/linux/cadog" "$OUTPUT_DIR/"
        ;;
esac

echo "[+] Release packaged at: $OUTPUT_DIR"
