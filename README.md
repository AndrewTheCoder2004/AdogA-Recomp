# Cadog Adventures – Portable Multi-Platform Recompilation

[![CI Matrix](https://img.shields.io/badge/CI-Multi--Platform-brightgreen.svg)](#supported-platforms)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

[![Platforms: 16](https://img.shields.io/badge/Platforms-16%20Supported-orange.svg)](#supported-platforms)

[![Offline: 100%](https://img.shields.io/badge/Offline-Zero%20Network%20Access-success.svg)](#offline-asset-extraction)

A modern portable open-source C++17 recompilation of the PC platformer Cadog Adventures (originally created by Niklas Wahrman / TA Studios, Assembly '04).

This repository is designed as an offline self-contained GitHub project. It requires no network connectivity to extract assets build or run. All game assets are cleanly extracted from the users original `setup.exe` installer via a built-in offline extractor and launcher.

---

## Supported Platforms (16 Targets)

Category  Platform  Target  Build Command  Binary Output

Desktop Windows 10 x86  `win32`  `scripts/build.sh win32`  `cadog.exe` (32-bit PE)

Windows 11 x64  `win64`  `scripts/build.sh win64`  `cadog.exe` (64-bit PE)

Linux (x86_64, arm64)  `linux`  `scripts/build.sh linux`  `cadog` (ELF)

BSD (FreeBSD, OpenBSD, NetBSD) bsd`  `scripts/build.sh bsd`  `cadog` (ELF) 

macOS (Universal arm64 + x86_64)  `macos`  `scripts/build.sh macos`  `cadog.app` 

HaikuOS  `haiku`  `scripts/build.sh haiku`  `cadog` 

ArcaOS / OS/2 Warp  `arcaos`  `scripts/build.sh arcaos`  `cadog.exe` 

Mobile  Android  `android`  `scripts/build.sh android`  APK (`com.cadog.game`) 

Apple iOS `ios`  `scripts/build.sh ios`  iOS App Bundle (`.ipa`) 

Windows Phone / WinRT / UWP  `winphone`  `scripts/build.sh winphone`  AppX Package 

Symbian OS (S60 v3/v5 Symbian^3) `symbian`  `scripts/build.sh symbian`  `cadog.sis` 

Console Nintendo Wii `wii`  `scripts/build.sh wii` cadog.dol` (Homebrew Channel) 

Microsoft Xbox 360 `xbox360`  `scripts/build.sh xbox360` `cadog.xex` 

Sony PlayStation 3  `ps3`  `scripts/build.sh ps3`  `EBOOT.BIN` / `.pkg` 

Nintendo 3DS `3ds`  `scripts/build.sh 3ds`  `cadog.3dsx` / `.cia` 

Sony PlayStation Vita `vita` scripts/build.sh vita`  `cadog.vpk` 

---

## Quick Start

### 1. Extract Assets & Run via Launcher

Place your legitimate `setup.exe` in the root repository directory (or specify its path):

```bash

# View all 65 original assets packed inside setup.exe:

python3 launcher.py --list

# Extract assets to the local 'assets folder without starting game:

python3 launcher.py --extract- --assets assets

# Build native binary and launch game automatically:

scripts/build.sh native

python3 launcher.py

```

### 2. Manual Build

If you have CMake and SDL2 installed on your system:

```bash

cmake -S. -B build/linux -DCMAKE_BUILD_TYPE=Release

cmake --build build/linux

./build/linux/cadog assets

```

---

 Input Emulation System

The game features a *Input Emulation Subsystem** that translates inputs from **keyboard** **gamepads / console controllers** and **on-screen touch overlays** into a unified set of abstract actions. Game logic runs identically across all desktop, console and mobile platforms.

Action Keyboard  Gamepad (Xbox / PS / PC)  Wii Controller  3DS / Vita  Mobile / Touch Screen 



 **Move Left**  Left Arrow / `A` D-Pad Left / Left Stick Wiimote Left / Stick  Circle Pad / D-Pad  Virtual `<` button 

 **Move Right** Right Arrow / `D` D-Pad Right / Left Stick  Wiimote Right / Stick| Circle Pad / D-Pad | >` button 

 **Look Up**  Up Arrow / `W`  D-Pad Up / Left Stick Wiimote Up / Stick  Circle Pad / D-Pad  Virtual `^` button 

 **Look Down**  Down Arrow / `S`  D-Pad Down / Left Stick  Wiimote Down / Stick  Circle Pad / D-Pad  Virtual `v. Button 

**Jump**  Space / `Z`  Button `A` / `B` (Cross) Wiimote `2` / Button `A`  Button `A` / `B`  Big Virtual `JUMP` Button 

**Action**  `X` / `C` / Shift Button `X` / `Y` (Square)  Wiimote `1` / Button `B` Button `X` / `Y`  Virtual `ACT` Button 

 **Start / Next Level** Enter / Return  Start Button  Wiimote `+` Start Button  Top-Right `START` 

 **Back / Exit**  Escape / Backspace | Back / Select Button  Wiimote `-` / Home  Select Button  Top- BACK` 

 Touch Controls Emulation Features

- **Auto-Detection:** Automatically enables the touch overlay on mobile platforms (**Android** **iOS** **Windows Phone** **Symbian**) handheld consoles (**3DS** bottom screen **PS Vita** OLED touch) or whenever a touch event is detected.

- **Desktop Simulation:** On PC platforms you can toggle the touch overlay on/off at any time by pressing `Tab` or `F1` and test with mouse clicks.

- **Visual Feedback:** Semi-transparent HUD overlay with golden-white illumination when pressed.

- **Analog Deadzone:** Gamepad analog sticks use an 8000-unit deadzone to prevent stick drift.

---

 Project Structure

```

├──.github/

│   └── workflows/           # Multi-platform CI pipeline

├── android/                       # Android Studio & Gradle project skeleton

│   ├── app/src/main/

│   │   ├── AndroidManifest.xml

│   │   └── java/com/cadog/game/CadogActivity.java

│   └── build.gradle

├── cmake/                         # Cross-compilation toolchains

│   ├── 3ds.cmake                  # Nintendo 3DS (devkitARM)

│   ├── arcaos.cmake               # ArcaOS / OS/2

│   ├── bsd.cmake                  # FreeBSD / OpenBSD / NetBSD

│   ├── haiku.cmake                # Haiku OS

│   ├── ios.toolchain.cmake        # Apple iOS

│   ├── mingw-w64-i686.cmake       # Windows 10 x86

│   ├── mingw-w64-x86_64.cmake     # Windows 11 x64

│   ├── ps3.cmake                  # PlayStation 3 (PSL1GHT)

│   ├── symbian.cmake              # Symbian OS (GCCE)

│   ├── vita.cmake                 # PlayStation Vita (VitaSDK)

│   ├── wii.cmake                  # Nintendo Wii (devkitPPC)

│   ├── winphone.cmake             # Windows Phone / WinRT / UWP

│   └── xbox360.cmake              # Microsoft Xbox 360 (libxenon)

├── console/                       # Console packaging descriptors

│   ├── 3ds/CadogAdventures.rsf

│   ├── ps3/package.cfg

│   ├── vita/template.xml

│   ├── wii/meta.xml

│   └── xbox360/launch.ini

├── docs/                          # In-depth documentation

│   ├── FORMATS.md                 # Reverse-engineered asset & level formats

│   ├── INPUT.md                   # Input Emulation subsystem architecture

│   ├── PLATFORMS.md               # 16-platform compilation and packaging guides

│   └── STATUS.md                  # Project status and implementation notes

├── ios/                           # Apple iOS project configuration

│   └── Info.plist

├── scripts/

│   ├── build.sh                   # Unified cross-platform build script

│   └── package_release.sh         # Offline release packager

├── src/

│   ├── audio.hpp / audio.cpp      # Zero-dependency SDL2 WAV mixer + OGG music

│   ├── formats.hpp                # RLE/raw TGA and,.clf level loader

│   ├── game.hpp / game.cpp        # Game loop, tilemap renderer, physics, entities

│   ├── input.hpp / input.cpp      # Input Emulation (Keyboard, Gamepad, Touch)

│   └── main.cpp                   # Main entry point & resolution scaling

├── symbian/                       # Symbian OS project files

│   ├── bld.inf

│   ├── cadog.mmp

│   └── cadog.pkg

├── tools/

│   └── cadog_extract.py           # Offline Clickteam Install Maker asset extractor

├── winphone/                      # Windows Phone / UWP package manifest

│   └── Package.appxmanifest

├── launcher.py                    # Crossplatform offline asset extractor & launcher

```

├── CMakeLists.txt                 # Unified CMake build definition for all 16 platforms

├── LICENSE                        # Open-source MIT License

└── setup.exe                      # original installer

```

---

 Technical Documentation

- [docs/PLATFORMS.md](docs/PLATFORMS.md): Step-by-step build guides, SDK setup and package generation for all 16 targets.

- [docs/INPUT.md](docs/Detailed explanation of the input emulation architecture.

- [docs/FORMATS.md](docs/FORMATS.md): Specification of the Clickteam installer overlay.clf level format and tile semantics.

- [docs/STATUS.md](docs/STATUS.md): Current implementation milestones.

---

 License

- **Recompilation Code:** Released under the MIT License.

- **Original Game & Assets:** © 2004 Niklas Wahrman / TA Studios (Assembly '04). Assets are property of their creator and are extracted locally by the user, from their legitimate setup.exe.

Make With SI.
