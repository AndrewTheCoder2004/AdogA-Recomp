#!/usr/bin/env bash
# Cadog Adventures - Cross-Platform Build Entry Point
# Supports 16 target platforms:
#   win32, win64, linux, bsd, macos, ios, winphone, android,
#   haiku, arcaos, wii, xbox360, ps3, 3ds, vita, symbian
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

TARGET="${1:-native}"

print_usage() {
    echo "Usage: scripts/build.sh <target> [build_type]"
    echo ""
    echo "Supported targets:"
    echo "  -- Desktop Platforms --"
    echo "  linux / native : Linux x86_64 / arm64 native build"
    echo "  bsd            : FreeBSD / OpenBSD / NetBSD build"
    echo "  win32 / win10  : Windows 10 x86 (MinGW-w64 32-bit)"
    echo "  win64 / win11  : Windows 11 x64 (MinGW-w64 64-bit)"
    echo "  macos          : macOS Universal Binary (arm64 + x86_64 .app bundle)"
    echo "  haiku          : Haiku OS native build"
    echo "  arcaos         : ArcaOS / OS/2 Warp build"
    echo ""
    echo "  -- Mobile Platforms --"
    echo "  android        : Android APK build (Gradle + NDK)"
    echo "  ios            : Apple iOS Xcode project / app bundle"
    echo "  winphone       : Windows Phone / WinRT / UWP package"
    echo "  symbian        : Symbian OS S60 / Symbian^3 SIS package"
    echo ""
    echo "  -- Console Platforms --"
    echo "  wii            : Nintendo Wii homebrew (devkitPPC .dol)"
    echo "  3ds            : Nintendo 3DS homebrew (devkitARM .3dsx / .cia)"
    echo "  vita           : PlayStation Vita (VitaSDK .vpk)"
    echo "  ps3            : PlayStation 3 (PSL1GHT EBOOT.BIN / .pkg)"
    echo "  xbox360        : Microsoft Xbox 360 (libxenon .xex)"
    echo ""
}

BUILD_TYPE="${2:-Release}"

echo "=================================================="
echo " Cadog Adventures Build System"
echo " Target:     $TARGET"
echo " Build Type: $BUILD_TYPE"
echo "=================================================="

case "$TARGET" in
  native|linux)
    mkdir -p build/linux
    cmake -S . -B build/linux -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/linux --config "$BUILD_TYPE" -j"$(nproc 2>/dev/null || echo 2)"
    echo "==> Build complete: build/linux/cadog"
    ;;

  bsd)
    mkdir -p build/bsd
    cmake -S . -B build/bsd -DCMAKE_TOOLCHAIN_FILE=cmake/bsd.cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/bsd --config "$BUILD_TYPE"
    echo "==> Build complete: build/bsd/cadog"
    ;;

  win32|win10)
    mkdir -p build/win32
    cmake -S . -B build/win32 -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-i686.cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/win32 --config "$BUILD_TYPE"
    echo "==> Build complete: build/win32/cadog.exe"
    ;;

  win64|win11)
    mkdir -p build/win64
    cmake -S . -B build/win64 -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-x86_64.cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/win64 --config "$BUILD_TYPE"
    echo "==> Build complete: build/win64/cadog.exe"
    ;;

  macos)
    mkdir -p build/macos
    cmake -S . -B build/macos -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
    cmake --build build/macos --config "$BUILD_TYPE"
    echo "==> Build complete: build/macos/cadog.app"
    ;;

  ios)
    mkdir -p build/ios
    cmake -S . -B build/ios -G Xcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.toolchain.cmake
    cmake --build build/ios --config "$BUILD_TYPE"
    echo "==> Build complete: build/ios"
    ;;

  android)
    echo "Building Android APK via Gradle wrapper..."
    cd android
    if [ -f "./gradlew" ]; then
      ./gradlew assembleRelease
    else
      gradle assembleRelease || echo "Run via Android Studio or install gradle"
    fi
    cd "$ROOT_DIR"
    echo "==> Android build finished"
    ;;

  winphone)
    mkdir -p build/winphone
    cmake -S . -B build/winphone -DCMAKE_TOOLCHAIN_FILE=cmake/winphone.cmake
    cmake --build build/winphone --config "$BUILD_TYPE"
    echo "==> Windows Phone build complete: build/winphone"
    ;;

  haiku)
    mkdir -p build/haiku
    cmake -S . -B build/haiku -DCMAKE_TOOLCHAIN_FILE=cmake/haiku.cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/haiku --config "$BUILD_TYPE"
    echo "==> Haiku build complete: build/haiku/cadog"
    ;;

  arcaos)
    mkdir -p build/arcaos
    cmake -S . -B build/arcaos -DCMAKE_TOOLCHAIN_FILE=cmake/arcaos.cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build build/arcaos --config "$BUILD_TYPE"
    echo "==> ArcaOS build complete: build/arcaos/cadog.exe"
    ;;

  wii)
    if [ -z "${DEVKITPRO:-}" ]; then
      echo "ERROR: DEVKITPRO environment variable is required for Wii build." >&2
      exit 1
    fi
    mkdir -p build/wii
    cmake -S . -B build/wii -DCMAKE_TOOLCHAIN_FILE=cmake/wii.cmake
    cmake --build build/wii
    echo "==> Wii build complete: build/wii/cadog.dol"
    ;;

  3ds)
    if [ -z "${DEVKITPRO:-}" ]; then
      echo "ERROR: DEVKITPRO environment variable is required for 3DS build." >&2
      exit 1
    fi
    mkdir -p build/3ds
    cmake -S . -B build/3ds -DCMAKE_TOOLCHAIN_FILE=cmake/3ds.cmake
    cmake --build build/3ds
    echo "==> 3DS build complete: build/3ds/cadog.3dsx"
    ;;

  vita)
    if [ -z "${VITASDK:-}" ]; then
      echo "ERROR: VITASDK environment variable is required for Vita build." >&2
      exit 1
    fi
    mkdir -p build/vita
    cmake -S . -B build/vita -DCMAKE_TOOLCHAIN_FILE=cmake/vita.cmake
    cmake --build build/vita
    echo "==> Vita build complete: build/vita/cadog.vpk"
    ;;

  ps3)
    if [ -z "${PS3DEV:-}" ]; then
      echo "ERROR: PS3DEV environment variable is required for PS3 build." >&2
      exit 1
    fi
    mkdir -p build/ps3
    cmake -S . -B build/ps3 -DCMAKE_TOOLCHAIN_FILE=cmake/ps3.cmake
    cmake --build build/ps3
    echo "==> PS3 build complete: build/ps3/EBOOT.BIN"
    ;;

  xbox360)
    mkdir -p build/xbox360
    cmake -S . -B build/xbox360 -DCMAKE_TOOLCHAIN_FILE=cmake/xbox360.cmake
    cmake --build build/xbox360
    echo "==> Xbox 360 build complete: build/xbox360/cadog.xex"
    ;;

  symbian)
    echo "Building for Symbian OS (S60 / Symbian^3)..."
    cd symbian
    if command -v bldmake >/dev/null 2>&1; then
      bldmake bldfiles
      abld build armv5 urel
      makesis cadog.pkg cadog.sis
      echo "==> Symbian package created: symbian/cadog.sis"
    else
      echo "Notice: Symbian SDK tools (bldmake/abld/makesis) not in PATH."
      echo "See docs/PLATFORMS.md for building with SBSv2 or Carbide.c++."
    fi
    cd "$ROOT_DIR"
    ;;

  help|-h|--help)
    print_usage
    exit 0
    ;;

  *)
    echo "Unknown target: $TARGET" >&2
    print_usage
    exit 1
    ;;
esac
