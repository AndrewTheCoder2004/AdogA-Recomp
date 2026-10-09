#pragma once
// Loaders for the original Cadog Adventures file formats.
#include <SDL.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// RLE / raw true-colour TGA (types 2 and 10, 24 or 32 bpp) -> RGBA32 surface.
inline SDL_Surface* loadTGA(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return nullptr;
    std::vector<uint8_t> d((std::istreambuf_iterator<char>(f)), {});
    if (d.size() < 18) return nullptr;
    const int idLen = d[0], type = d[2], w = d[12] | d[13] << 8, h = d[14] | d[15] << 8;
    const int bpp = d[16] / 8;
    const bool topDown = d[17] & 0x20;
    if ((type != 2 && type != 10) || (bpp != 3 && bpp != 4) || !w || !h) return nullptr;
    size_t p = 18 + idLen;
    std::vector<uint8_t> px(size_t(w) * h * 4);
    size_t n = 0;
    auto put = [&](const uint8_t* s) {  // BGR(A) -> RGBA
        if (n >= px.size()) return;
        px[n++] = s[2]; px[n++] = s[1]; px[n++] = s[0]; px[n++] = bpp == 4 ? s[3] : 255;
    };
    while (n < px.size() && p < d.size()) {
        if (type == 2) {
            if (p + bpp > d.size()) break;
            put(&d[p]); p += bpp;
        } else {
            const int hdr = d[p++], cnt = (hdr & 0x7f) + 1;
            if (hdr & 0x80) {
                if (p + bpp > d.size()) break;
                for (int i = 0; i < cnt; i++) put(&d[p]);
                p += bpp;
            } else {
                for (int i = 0; i < cnt && p + bpp <= d.size(); i++, p += bpp) put(&d[p]);
            }
        }
    }
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!s) return nullptr;
    for (int y = 0; y < h; y++) {
        const int srcY = topDown ? y : h - 1 - y;
        std::copy_n(&px[size_t(srcY) * w * 4], size_t(w) * 4, (uint8_t*)s->pixels + size_t(y) * s->pitch);
    }
    return s;
}

// .clf level: u32 width, then width columns of kLevelRows u32 tile ids.
// (Layout derived from file sizes: 4 + width * 27 * 4 for all 16 shipped levels.)
constexpr int kLevelRows = 27;
struct Level {
    int width = 0;
    std::vector<uint32_t> tiles;  // column-major: tiles[x * kLevelRows + y]
    uint32_t at(int x, int y) const {
        return (x < 0 || y < 0 || x >= width || y >= kLevelRows) ? 0 : tiles[size_t(x) * kLevelRows + y];
    }
};

inline bool loadLevel(const std::string& path, Level& out) {
    std::ifstream f(path, std::ios::binary);
    uint32_t w = 0;
    if (!f.read(reinterpret_cast<char*>(&w), 4) || w == 0 || w > 4096) return false;
    out.width = int(w);
    out.tiles.assign(size_t(w) * kLevelRows, 0);
    return bool(f.read(reinterpret_cast<char*>(out.tiles.data()), out.tiles.size() * 4));
}
