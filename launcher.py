#!/usr/bin/env python3
"""Cadog Adventures Launcher & Offline Asset Extractor.

Extracts game assets from the legitimate PC `setup.exe` installer
and runs the native game binary for the platform.
Completely offline: zero network access required.

Usage:
    python3 launcher.py [path/to/setup.exe] [--assets DIR] [--extract-only]
"""
import argparse
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "tools"))
import cadog_extract  # noqa: E402


def default_data_dir():
    # If a local assets directory exists with game files, prioritize it
    local_assets = os.path.join(HERE, "assets")
    if os.path.isfile(os.path.join(local_assets, "L0.clf")):
        return local_assets

    if sys.platform.startswith("win"):
        base = os.environ.get("APPDATA", os.path.expanduser("~"))
    elif sys.platform == "darwin":
        base = os.path.expanduser("~/Library/Application Support")
    else:
        base = os.environ.get("XDG_DATA_HOME", os.path.expanduser("~/.local/share"))
    return os.path.join(base, "CadogAdventures", "assets")


def find_setup(user_path=None):
    candidates = []
    if user_path:
        candidates.append(user_path)
    candidates.extend([
        os.path.join(os.getcwd(), "setup.exe"),
        os.path.join(HERE, "setup.exe"),
        os.path.join(HERE, "installer", "setup.exe"),
    ])
    for c in candidates:
        if os.path.isfile(c):
            return c
    return None


def find_game_binary():
    names = ["cadog.exe", "cadog"] if sys.platform.startswith("win") else ["cadog", "cadog.exe"]
    search_dirs = [
        HERE,
        os.path.join(HERE, "bin"),
        os.path.join(HERE, "build"),
        os.path.join(HERE, "build", "native"),
        os.path.join(HERE, "build", "linux"),
        os.path.join(HERE, "build", "win64"),
        os.path.join(HERE, "build", "win32"),
        os.path.join(HERE, "build", "macos"),
        os.path.join(HERE, "build", "Release"),
        os.path.join(HERE, "build", "Debug"),
    ]
    for d in search_dirs:
        for name in names:
            p = os.path.join(d, name)
            if os.path.isfile(p):
                return p
    return None


def display_message(text, is_error=False):
    stream = sys.stderr if is_error else sys.stdout
    print(text, file=stream)
    try:
        import tkinter
        from tkinter import messagebox
        root = tkinter.Tk()
        root.withdraw()
        if is_error:
            messagebox.showerror("Cadog Adventures", text)
        else:
            messagebox.showinfo("Cadog Adventures", text)
        root.destroy()
    except Exception:
        pass


def verify_extracted_assets(assets_dir):
    required = ["L0.clf", "tiles_0_0.tga", "player.tga", "background1.tga"]
    for r in required:
        if not os.path.isfile(os.path.join(assets_dir, r)):
            return False
    return True


def main():
    parser = argparse.ArgumentParser(description="Cadog Adventures Launcher & Asset Manager")
    parser.add_argument("installer", nargs="?", default=None, help="Path to legitimate setup.exe")
    parser.add_argument("--assets", default=None, help="Directory to extract/find assets")
    parser.add_argument("--extract-only", action="store_true", help="Only extract assets without starting the game")
    parser.add_argument("--list", action="store_true", help="List files inside setup.exe without extracting")
    args = parser.parse_args()

    setup_file = find_setup(args.installer)

    if args.list:
        if not setup_file:
            display_message("setup.exe not found to list.", is_error=True)
            return 1
        return cadog_extract.main(["tools/cadog_extract.py", setup_file, "", "--list"])

    assets_dir = args.assets or default_data_dir()

    # If assets are not ready, extract from setup.exe
    if not verify_extracted_assets(assets_dir):
        if not setup_file:
            display_message(
                "Missing assets! Please place your legitimate Cadog Adventures 'setup.exe'\n"
                "in this folder or pass its location: python3 launcher.py /path/to/setup.exe",
                is_error=True
            )
            return 1

        print(f"[*] Extracting assets from '{setup_file}' to '{assets_dir}'...")
        res = cadog_extract.main(["tools/cadog_extract.py", setup_file, assets_dir])
        if res != 0 or not verify_extracted_assets(assets_dir):
            display_message("Failed to extract assets. Ensure setup.exe is valid.", is_error=True)
            return 1
        print("[+] Assets extracted successfully.")

    if args.extract_only:
        print(f"[+] All assets verified at: {assets_dir}")
        return 0

    # Locate compiled executable
    game_bin = find_game_binary()
    if not game_bin:
        display_message(
            "Game executable 'cadog' not found!\n"
            "Build the game for your platform first:\n"
            "  scripts/build.sh native   (Linux/BSD/Haiku)\n"
            "  scripts/build.sh win64    (Windows 64-bit)\n"
            "  scripts/build.sh macos    (macOS)",
            is_error=True
        )
        return 1

    print(f"[*] Launching {game_bin} with assets {assets_dir}...")
    try:
        return subprocess.call([game_bin, assets_dir])
    except KeyboardInterrupt:
        return 0
    except Exception as e:
        display_message(f"Error launching game: {e}", is_error=True)
        return 1


if __name__ == "__main__":
    sys.exit(main())
