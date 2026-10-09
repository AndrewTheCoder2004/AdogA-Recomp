# Platform Build and Compilation Guide

This document describes how to compile **Cadog Adventures** for each of the 16 supported platforms.
All platform builds are completely self-contained and run offline.

---

## Supported Platforms Matrix

| # | Platform | Target ID | Toolchain / SDK | Binary Output |
|---|----------|-----------|-----------------|---------------|
| 1 | **Windows 10 x86** | `win32` | MinGW-w64 (`i686-w64-mingw32`) / MSVC x86 | `cadog.exe` (32-bit PE) |
| 2 | **Windows 11 x64** | `win64` | MinGW-w64 (`x86_64-w64-mingw32`) / MSVC x64 | `cadog.exe` (64-bit PE) |
| 3 | **Linux** | `linux` / `native` | GCC / Clang + SDL2 | `cadog` (ELF) |
| 4 | **BSD** (FreeBSD, OpenBSD, NetBSD) | `bsd` | Clang + BSD port SDL2 | `cadog` (ELF) |
| 5 | **macOS** | `macos` | Clang (Apple Silicon + Intel Universal) | `cadog.app` bundle |
| 6 | **iOS** | `ios` | Xcode + iOS SDK + CMake | iOS App Bundle (`.ipa`) |
| 7 | **Windows Phone** | `winphone` | Visual Studio WinRT / UWP ARM or x86 | AppX package |
| 8 | **Android** | `android` | Android NDK + Gradle (`SDLActivity`) | APK / AAB |
| 9 | **HaikuOS** | `haiku` | Haiku GCC (`sdl2_devel`) | `cadog` (Haiku binary) |
| 10 | **ArcaOS / OS/2** | `arcaos` | GCC for OS/2 (EMX) + OS/2 SDL2 | `cadog.exe` (OS/2 LX format) |
| 11 | **Nintendo Wii** | `wii` | devkitPro (`devkitPPC`) + `libogc` + `wii-sdl2` | `cadog.dol` (Homebrew Channel) |
| 12 | **Microsoft Xbox 360** | `xbox360` | `libxenon` toolchain | `cadog.xex` |
| 13 | **Sony PlayStation 3** | `ps3` | `PSL1GHT` + `SDL2-ps3` | `EBOOT.BIN` / `.pkg` |
| 14 | **Nintendo 3DS** | `3ds` | devkitPro (`devkitARM`) + `libctru` + `3ds-sdl2` | `cadog.3dsx` / `.cia` |
| 15 | **Sony PlayStation Vita** | `vita` | `VitaSDK` + `vita-sdl2` | `cadog.vpk` |
| 16 | **Symbian OS** (S60 v3/v5, Symbian^3) | `symbian` | GCCE / SBSv2 + Open C P.I.P.S. + SDL | `cadog.sis` |

---

## Desktop Platforms

### 1. Windows 10 x86
- **Cross-compiling from Linux:**
  ```bash
  sudo apt-get install mingw-w64
  scripts/build.sh win32
  ```
- **Building natively on Windows:**
  ```cmd
  cmake -B build\win32 -A Win32 -DCMAKE_BUILD_TYPE=Release
  cmake --build build\win32 --config Release
  ```

### 2. Windows 11 x64
- **Cross-compiling from Linux:**
  ```bash
  sudo apt-get install mingw-w64
  scripts/build.sh win64
  ```
- **Building natively on Windows:**
  ```cmd
  cmake -B build\win64 -A x64 -DCMAKE_BUILD_TYPE=Release
  cmake --build build\win64 --config Release
  ```

### 3. Linux (x86_64 / arm64)
- **Requirements:** CMake 3.16+, C++17 compiler (`g++` or `clang++`), `libsdl2-dev`, optional `libsdl2-mixer-dev`.
  ```bash
  sudo apt-get install cmake build-essential libsdl2-dev libsdl2-mixer-dev
  scripts/build.sh linux
  ```

### 4. BSD (FreeBSD, OpenBSD, NetBSD)
- **Requirements:** Clang, SDL2 port.
  ```bash
  # FreeBSD:
  pkg install cmake sdl2
  scripts/build.sh bsd
  ```

### 5. macOS (Universal Binary)
- Builds universal binary supporting both Apple Silicon (M1/M2/M3/M4) and Intel x86_64:
  ```bash
  brew install sdl2 cmake
  scripts/build.sh macos
  ```
- Produces `build/macos/cadog.app`.

### 9. HaikuOS
- **Requirements:** Install `sdl2_devel` and `cmake` via HaikuDepot or `pkgman`:
  ```bash
  pkgman install sdl2_devel cmake
  scripts/build.sh haiku
  ```

### 10. ArcaOS / OS/2 Warp
- **Requirements:** Netlabs GCC toolchain with OS/2 SDL2 port (`sdl2-os2`).
  ```bash
  scripts/build.sh arcaos
  ```

---

## Mobile Platforms

### 6. Apple iOS
- Generates an Xcode project configured with touch inputs enabled:
  ```bash
  scripts/build.sh ios
  ```
- Open `build/ios/CadogAdventures.xcodeproj` in Xcode to archive, sign, and install.

### 7. Windows Phone / Windows 10 Mobile
- **Requirements:** Visual Studio with UWP / C++ WindowsStore components.
  ```cmd
  cmake -S . -B build/winphone -DCMAKE_TOOLCHAIN_FILE=cmake/winphone.cmake
  cmake --build build/winphone
  ```
- Includes `winphone/Package.appxmanifest` configured for mobile landscape resolution.

### 8. Android
- **Requirements:** Android Studio or command-line Gradle + Android NDK.
  ```bash
  scripts/build.sh android
  ```
- The project in `android/` links the C++ engine (`libcadog.so`) with `SDLActivity`. Touch overlay is automatically activated.

### 16. Symbian OS (S60 3rd/5th Edition, Symbian^3)
- **Requirements:** Symbian SDK (e.g. S60 5th Ed SDK), GCCE compiler, Open C/C++ P.I.P.S. libraries, SDL 1.2/2 port for S60.
- Project files are in `symbian/`:
  - `symbian/bld.inf`: Project build registry.
  - `symbian/cadog.mmp`: Component definition linking POSIX C/C++ and SDL.
  - `symbian/cadog.pkg`: Package specification to build `cadog.sis`.
- Command line build:
  ```bash
  cd symbian
  bldmake bldfiles
  abld build armv5 urel
  makesis cadog.pkg cadog.sis
  ```

---

## Console Platforms

### 11. Nintendo Wii
- **Requirements:** devkitPro (`devkitPPC`), `libogc`, `wii-sdl2` portlib.
  ```bash
  export DEVKITPRO=/opt/devkitpro
  scripts/build.sh wii
  ```
- Generates `build/wii/cadog.dol`. Copy `cadog.dol`, `console/wii/meta.xml`, and the `assets/` folder to `/apps/CadogAdventures/` on your SD card.

### 12. Microsoft Xbox 360
- **Requirements:** `libxenon` open toolchain and `xenonxex`.
  ```bash
  export DEVKITXENON=/usr/local/xenon
  scripts/build.sh xbox360
  ```
- Produces `cadog.xex`. Copy with `console/xbox360/launch.ini` to your Xbox 360 hard drive.

### 13. Sony PlayStation 3
- **Requirements:** `PSL1GHT` open homebrew SDK and `SDL2-ps3`.
  ```bash
  export PS3DEV=/usr/local/ps3dev
  scripts/build.sh ps3
  ```
- Generates `EBOOT.BIN`. Package with `console/ps3/package.cfg` to create installable `.pkg`.

### 14. Nintendo 3DS
- **Requirements:** devkitPro (`devkitARM`), `libctru`, `3ds-sdl2`.
  ```bash
  export DEVKITPRO=/opt/devkitpro
  scripts/build.sh 3ds
  ```
- Generates `cadog.3dsx` and `.cia` (using `console/3ds/CadogAdventures.rsf`). Circle Pad, buttons, and bottom touchscreen are fully mapped.

### 15. Sony PlayStation Vita
- **Requirements:** `VitaSDK` (`vitasdk`), `vita-sdl2`.
  ```bash
  export VITASDK=/usr/local/vitasdk
  scripts/build.sh vita
  ```
- Generates `cadog.vpk` ready to install via VitaShell.
