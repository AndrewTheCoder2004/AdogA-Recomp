# Reverse-Engineered File Formats

This document details the internal specifications of the Cadog Adventures asset containers and data structures recovered through binary disassembly of the original `setup.exe` and `cadog.exe`.

---

## 1. Installer Package (`setup.exe`)

- **Authoring Engine:** Clickteam Install Maker.
- **Structure:**
  - Standard 32-bit Portable Executable (PE) stub (`0x00000` to `0x20000`).
  - Overlay starting at offset `0x20000` containing compressed asset banks.
- **Compression Schemes:**
  - **Bzip2 banks:** Prefixed with standard magic header `BZh1` through `BZh9` (`0x42 0x5A 0x68 ...`). Used for textures, sounds, and level files.
  - **Zlib streams:** Prefixed with standard zlib headers (`0x78 ...`). Used for OGG music tracks and documentation.
- **Bank Directory Header:**
  ```c
  struct BankHeader {
      uint32_t entry_count;
      struct BankEntry {
          char     filename[32];   // Null-padded ASCII filename
          uint32_t size;           // Uncompressed byte size
          uint32_t offset;         // Byte offset relative to end of entry table
          uint32_t reserved;
      } entries[entry_count];
      uint8_t payload[];
  };
  ```

---

## 2. Level Format (`*.clf`)

- **Header:**
  ```c
  struct LevelHeader {
      uint32_t width;             // Level width in tiles (e.g. 128, 200, 256, 320, 400)
  };
  ```
- **Tile Grid:**
  - Fixed vertical height: **27 tiles** (`kLevelRows = 27`).
  - Tile storage: **Column-major order**, 32-bit unsigned integer per tile:
    ```
    total_tiles = width * 27
    offset_in_file = 4 + (x * 27 + y) * 4
    ```
- **Tile ID Semantics:**
  - `0`: Air / Empty space.
  - `1 .. 39`: Terrain blocks, platforms, ledges, and slopes.
  - `40 .. 43`: Collectible Coins (cleared from map upon level load and instantiated as animated coin entities).
  - `44`: Checkpoint Sign (updates player respawn coordinates).
  - `45`: Level Exit Sign (completes level upon contact).
  - `54 .. 62`: Enemies (Spikey, Blob, Cocoa, Parrot; cleared from map and instantiated as patrolling entities).

---

## 3. Graphics & Textures (`*.tga`)

- **Format:** Truevision TGA (Type 2: uncompressed true-color, or Type 10: Run-Length Encoded true-color).
- **Color Depth:** 24-bit BGR or 32-bit BGRA.
- **Dimensions:** Power-of-two dimensions (128x128, 256x256, 512x512).
- **Tile Sheets (`tiles_0_0.tga`, etc.):**
  - Dimensions: 512 x 512 pixels.
  - Layout: 8 x 8 grid of **64 x 64** pixel tiles.
  - Slice calculation:
    ```c
    int row = tile_id / 8;
    int col = tile_id % 8;
    SDL_Rect src = { col * 64, row * 64, 64, 64 };
    ```
- **Sprites:**
  - `player.tga`: Cadog animations (walk, jump, idle).
  - `Coins.tga`: 128 x 128 sheet with 4 animation rotation frames.
  - `Enemy1.tga` / `Enemy2.tga` / `Enemy3.tga`: Enemy sprites.
  - `Numbers.tga` / `life.tga`: HUD display elements.

---

## 4. Audio Files

- **Sound Effects (WAV):**
  - PCM uncompressed 16-bit mono/stereo audio:
    `sndJump.wav`, `sndCoin.wav`, `sndEnemy.wav`, `sndHit.wav`, `sndCheck.wav`,
    `sndLevelCompleted.wav`, `sndGameOver.wav`, `sndGameWon.wav`, `sndSelect.wav`, `sndBack.wav`.
- **Music (OGG):**
  - Ogg Vorbis compressed audio streams:
    `Music1.ogg`, `Music2.ogg`, `Music3.ogg`, `Menu.ogg`.
