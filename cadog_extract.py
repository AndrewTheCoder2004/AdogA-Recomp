#!/usr/bin/env python3
"""Offline extractor for the Cadog Adventures `setup.exe` (Clickteam Install Maker).

Reads the user's own installer, locates the asset banks inside it and writes the
files to an output directory. No network access, no third-party dependencies.

Bank layout (little endian), found by analysing the installer:
    u32 count
    count * { char name[32]; u32 size; u32 offset; u32 reserved; }
    payload   (offsets are relative to the end of the entry table)
Banks are stored either as raw zlib streams (music bank) or as bzip2 streams
preceded by a 1 byte flag (graphics / sound banks).

Usage: cadog_extract.py setup.exe OUTDIR [--list]
"""
import bz2
import os
import re
import struct
import sys
import zlib

ENTRY = 44
OVERLAY_START = 0x20000  # PE image ends at 0x20000 in the known installer


def parse_bank(blob):
    """Return {name: bytes} if blob looks like an asset bank, else None."""
    if len(blob) < 4:
        return None
    (count,) = struct.unpack_from("<I", blob, 0)
    if not 0 < count < 4096 or 4 + count * ENTRY > len(blob):
        return None
    base = 4 + count * ENTRY
    files = {}
    for i in range(count):
        raw = blob[4 + i * ENTRY: 4 + (i + 1) * ENTRY]
        name = raw[:32].split(b"\0")[0].decode("latin1")
        size, off, _ = struct.unpack_from("<III", raw, 32)
        if not re.fullmatch(r"[\w .\-]+", name) or base + off + size > len(blob):
            return None
        files[name] = blob[base + off: base + off + size]
    return files


def find_banks(data):
    banks = []
    seen = set()
    # bzip2 streams (may be concatenated back to back)
    for m in re.finditer(rb"BZh[1-9]\x31\x41\x59\x26\x53\x59", data[OVERLAY_START:]):
        pos = OVERLAY_START + m.start()
        if pos in seen:
            continue
        try:
            dec = bz2.BZ2Decompressor()
            out = dec.decompress(data[pos:])
            if not dec.eof:
                continue
        except (OSError, ValueError):
            continue
        bank = parse_bank(out)
        if bank:
            banks.append(bank)
            seen.add(pos)
    # zlib streams
    for m in re.finditer(rb"\x78[\x01\x5e\x9c\xda]", data[OVERLAY_START:]):
        pos = OVERLAY_START + m.start()
        try:
            dec = zlib.decompressobj()
            out = dec.decompress(data[pos:])
            if not dec.eof:
                continue
        except zlib.error:
            continue
        bank = parse_bank(out)
        if bank:
            banks.append(bank)
        elif out[:2] == b"MZ" and len(out) > 300000:
            banks.append({"__original_game.exe": out})  # reference only
        elif out.startswith(b"\r\n\tCadog"):
            banks.append({"readme.txt": out})
    return banks


def carve_ogg(blob):
    """Music bank entries hold raw .ogg data; nothing to carve, kept for sanity."""
    return blob.startswith(b"OggS")


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    src, out = argv[1], argv[2]
    with open(src, "rb") as f:
        data = f.read()
    if data[:2] != b"MZ":
        print("not a PE executable", file=sys.stderr)
        return 1
    banks = find_banks(data)
    if not banks:
        print("no asset banks found - is this the Cadog Adventures installer?", file=sys.stderr)
        return 1
    total = 0
    for bank in banks:
        for name, blob in bank.items():
            total += 1
            if "--list" in argv:
                print("%8d  %s" % (len(blob), name))
                continue
            dest = os.path.join(out, name)
            os.makedirs(os.path.dirname(dest) or ".", exist_ok=True)
            with open(dest, "wb") as f:
                f.write(blob)
    print("%d files %s" % (total, "listed" if "--list" in argv else "extracted to " + out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
