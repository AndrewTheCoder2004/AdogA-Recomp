#include "game.hpp"
#include <cstdio>
#include <algorithm>
#include <cmath>

static SDL_Texture* loadTexture(SDL_Renderer* ren, const std::string& path) {
    SDL_Surface* surf = loadTGA(path);
    if (!surf) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}

Game::Game()
    : running_(true),
      currentLevelIdx_(0),
      px_(64.0f), py_(300.0f),
      pvx_(0.0f), pvy_(0.0f),
      onGround_(false), facingRight_(true),
      lives_(3), score_(0),
      invulnerableTimer_(0.0f),
      spawnX_(64.0f), spawnY_(300.0f),
      camX_(0), camY_(0),
      levelWon_(false), winTimer_(0.0f),
      gameOver_(false), gameOverTimer_(0.0f) {}

Game::~Game() {
    if (texBg_) SDL_DestroyTexture(texBg_);
    if (texTiles_) SDL_DestroyTexture(texTiles_);
    if (texPlayer_) SDL_DestroyTexture(texPlayer_);
    if (texPlayer2_) SDL_DestroyTexture(texPlayer2_);
    if (texCoins_) SDL_DestroyTexture(texCoins_);
    if (texSigns_) SDL_DestroyTexture(texSigns_);
    if (texCheckpoint_) SDL_DestroyTexture(texCheckpoint_);
    if (texEnemies1_) SDL_DestroyTexture(texEnemies1_);
    if (texEnemies2_) SDL_DestroyTexture(texEnemies2_);
    if (texLife_) SDL_DestroyTexture(texLife_);
    if (texNumbers_) SDL_DestroyTexture(texNumbers_);
    if (texVictory_) SDL_DestroyTexture(texVictory_);
}

bool Game::init(SDL_Renderer* ren, const std::string& assets) {
    assetsPath_ = assets;

    // Load authentic extracted textures
    texBg_         = loadTexture(ren, assets + "/background1.tga");
    texTiles_      = loadTexture(ren, assets + "/tiles_0_0.tga");
    texPlayer_     = loadTexture(ren, assets + "/player.tga");
    texPlayer2_    = loadTexture(ren, assets + "/player2.tga");
    texCoins_      = loadTexture(ren, assets + "/Coins.tga");
    texSigns_      = loadTexture(ren, assets + "/Signs.tga");
    texCheckpoint_ = loadTexture(ren, assets + "/checkpoint.tga");
    texEnemies1_   = loadTexture(ren, assets + "/Enemy1.tga");
    texEnemies2_   = loadTexture(ren, assets + "/Enemy2.tga");
    texLife_       = loadTexture(ren, assets + "/life.tga");
    texNumbers_    = loadTexture(ren, assets + "/Numbers.tga");
    texVictory_    = loadTexture(ren, assets + "/VictoryScreen.tga");

    // Initialize audio system
    audio_.init(assets);

    // Load first level
    loadLevelIndex(0);

    return true;
}

void Game::loadLevelIndex(int idx) {
    currentLevelIdx_ = (idx >= 0 && idx < 16) ? idx : 0;
    std::string levelPath = assetsPath_ + "/L" + std::to_string(currentLevelIdx_) + ".clf";
    if (!loadLevel(levelPath, level_)) {
        std::fprintf(stderr, "Failed to load level %s\n", levelPath.c_str());
        return;
    }

    levelWon_ = false;
    winTimer_ = 0.0f;
    gameOver_ = false;

    // Scan level tiles and instantiate entities
    initEntitiesFromMap();

    px_ = spawnX_;
    py_ = spawnY_;
    pvx_ = 0.0f;
    pvy_ = 0.0f;
    onGround_ = false;
}

void Game::nextLevel() {
    loadLevelIndex((currentLevelIdx_ + 1) % 16);
}

void Game::restartLevel() {
    loadLevelIndex(currentLevelIdx_);
}

void Game::initEntitiesFromMap() {
    entities_.clear();
    spawnX_ = 64.0f;
    spawnY_ = 250.0f;

    for (int x = 0; x < level_.width; ++x) {
        for (int y = 0; y < kLevelRows; ++y) {
            uint32_t tid = level_.at(x, y);
            if (tid == 0) continue;

            const float worldX = float(x * TILE_SIZE);
            const float worldY = float(y * TILE_SIZE);

            // Coins (tile 40 to 43)
            if (tid >= 40 && tid <= 43) {
                Entity e;
                e.type = ENT_COIN;
                e.x = worldX;
                e.y = worldY;
                e.width = 24;
                e.height = 24;
                entities_.push_back(e);
                // Clear map tile so terrain renderer doesn't draw it
                level_.tiles[size_t(x) * kLevelRows + y] = 0;
            }
            // Checkpoint Sign (tile 44)
            else if (tid == 44) {
                Entity e;
                e.type = ENT_CHECKPOINT;
                e.x = worldX;
                e.y = worldY;
                e.width = 32;
                e.height = 32;
                entities_.push_back(e);
                level_.tiles[size_t(x) * kLevelRows + y] = 0;
            }
            // Exit Sign (tile 45)
            else if (tid == 45) {
                Entity e;
                e.type = ENT_EXIT;
                e.x = worldX;
                e.y = worldY;
                e.width = 32;
                e.height = 32;
                entities_.push_back(e);
                level_.tiles[size_t(x) * kLevelRows + y] = 0;
            }
            // Enemies (tiles 54 to 62)
            else if (tid >= 54 && tid <= 62) {
                Entity e;
                e.type = (tid % 2 == 0) ? ENT_ENEMY_SPIKEY : ENT_ENEMY_BLOB;
                e.x = worldX;
                e.y = worldY;
                e.vx = (tid % 2 == 0) ? -1.2f : 1.0f;
                e.width = 28;
                e.height = 28;
                entities_.push_back(e);
                level_.tiles[size_t(x) * kLevelRows + y] = 0;
            }
        }
    }
}

void Game::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        running_ = false;
    }
    input_.handleEvent(e);
}

bool Game::isSolidTile(int tx, int ty) const {
    if (tx < 0 || tx >= level_.width || ty < 0 || ty >= kLevelRows) {
        return false;
    }
    uint32_t tid = level_.at(tx, ty);
    // Authentic solid tiles: tile IDs 1..39
    return (tid >= 1 && tid <= 39);
}

void Game::updatePlayer(float dt) {
    const float ACCEL = 900.0f;
    const float FRICTION = 0.85f;
    const float MAX_SPEED = 240.0f;
    const float GRAVITY = 1100.0f;
    const float JUMP_FORCE = -480.0f;

    // Movement
    if (input_.down(A_LEFT)) {
        pvx_ -= ACCEL * dt;
        facingRight_ = false;
    } else if (input_.down(A_RIGHT)) {
        pvx_ += ACCEL * dt;
        facingRight_ = true;
    } else {
        pvx_ *= std::pow(FRICTION, dt * 60.0f);
    }
    pvx_ = std::clamp(pvx_, -MAX_SPEED, MAX_SPEED);

    // Jump
    if (input_.pressed(A_JUMP) && onGround_) {
        pvy_ = JUMP_FORCE;
        onGround_ = false;
        audio_.playSound(SFX_JUMP);
    }

    // Gravity
    pvy_ += GRAVITY * dt;
    if (pvy_ > 600.0f) pvy_ = 600.0f;

    // Horizontal movement & collision
    px_ += pvx_ * dt;
    int ptx1 = int(px_) / TILE_SIZE;
    int ptx2 = int(px_ + 24) / TILE_SIZE;
    int pty1 = int(py_ + 4) / TILE_SIZE;
    int pty2 = int(py_ + 28) / TILE_SIZE;

    if (pvx_ > 0 && (isSolidTile(ptx2, pty1) || isSolidTile(ptx2, pty2))) {
        px_ = float(ptx2 * TILE_SIZE - 25);
        pvx_ = 0;
    } else if (pvx_ < 0 && (isSolidTile(ptx1, pty1) || isSolidTile(ptx1, pty2))) {
        px_ = float((ptx1 + 1) * TILE_SIZE);
        pvx_ = 0;
    }

    // Vertical movement & collision
    py_ += pvy_ * dt;
    ptx1 = int(px_ + 4) / TILE_SIZE;
    ptx2 = int(px_ + 20) / TILE_SIZE;
    pty1 = int(py_) / TILE_SIZE;
    pty2 = int(py_ + 32) / TILE_SIZE;

    onGround_ = false;
    if (pvy_ > 0 && (isSolidTile(ptx1, pty2) || isSolidTile(ptx2, pty2))) {
        py_ = float(pty2 * TILE_SIZE - 32);
        pvy_ = 0;
        onGround_ = true;
    } else if (pvy_ < 0 && (isSolidTile(ptx1, pty1) || isSolidTile(ptx2, pty1))) {
        py_ = float((pty1 + 1) * TILE_SIZE);
        pvy_ = 0;
    }

    // World bounds
    px_ = std::clamp(px_, 0.0f, float(level_.width * TILE_SIZE - 32));

    // Fall below map -> respawn
    if (py_ > float(kLevelRows * TILE_SIZE + 64)) {
        lives_--;
        audio_.playSound(SFX_HIT);
        if (lives_ <= 0) {
            gameOver_ = true;
            gameOverTimer_ = 3.0f;
            audio_.playSound(SFX_GAME_OVER);
        } else {
            px_ = spawnX_;
            py_ = spawnY_;
            pvx_ = 0;
            pvy_ = 0;
        }
    }

    if (invulnerableTimer_ > 0.0f) {
        invulnerableTimer_ -= dt;
    }
}

void Game::updateEntities(float dt) {
    for (auto& e : entities_) {
        if (!e.active) continue;

        e.animTimer += dt;
        if (e.animTimer >= 0.15f) {
            e.animTimer = 0.0f;
            e.animFrame = (e.animFrame + 1) % 4;
        }

        // Enemy movement
        if (e.type == ENT_ENEMY_SPIKEY || e.type == ENT_ENEMY_BLOB) {
            e.x += e.vx * dt * 50.0f;
            int tx = int(e.x + (e.vx > 0 ? e.width : 0)) / TILE_SIZE;
            int ty = int(e.y + e.height / 2) / TILE_SIZE;
            if (isSolidTile(tx, ty) || e.x <= 0 || e.x >= level_.width * TILE_SIZE) {
                e.vx = -e.vx;
            }
        }
    }
}

void Game::checkCollisions() {
    SDL_Rect pr{int(px_), int(py_), 24, 32};

    for (auto& e : entities_) {
        if (!e.active) continue;

        SDL_Rect er{int(e.x), int(e.y), e.width, e.height};
        if (!SDL_HasIntersection(&pr, &er)) continue;

        // Coin
        if (e.type == ENT_COIN) {
            e.active = false;
            score_ += 100;
            audio_.playSound(SFX_COIN);
        }
        // Checkpoint
        else if (e.type == ENT_CHECKPOINT && !e.triggered) {
            e.triggered = true;
            spawnX_ = e.x;
            spawnY_ = e.y - 10;
            audio_.playSound(SFX_CHECK);
        }
        // Exit
        else if (e.type == ENT_EXIT && !levelWon_) {
            levelWon_ = true;
            winTimer_ = 3.0f;
            audio_.playSound(SFX_LEVEL_COMPLETED);
        }
        // Enemy
        else if (e.type == ENT_ENEMY_SPIKEY || e.type == ENT_ENEMY_BLOB) {
            // Check if player is stomping from above
            if (pvy_ > 0 && (py_ + 24) < (e.y + 12)) {
                e.active = false;
                pvy_ = -320.0f; // Bounce
                score_ += 200;
                audio_.playSound(SFX_ENEMY);
            } else if (invulnerableTimer_ <= 0.0f) {
                // Take hit
                lives_--;
                invulnerableTimer_ = 1.5f;
                audio_.playSound(SFX_HIT);
                pvy_ = -200.0f;
                pvx_ = facingRight_ ? -150.0f : 150.0f;

                if (lives_ <= 0) {
                    gameOver_ = true;
                    gameOverTimer_ = 3.0f;
                    audio_.playSound(SFX_GAME_OVER);
                }
            }
        }
    }
}

void Game::update(float dt) {
    input_.update();

    // Menu / Navigation actions
    if (input_.pressed(A_BACK)) {
        running_ = false;
        return;
    }
    if (input_.pressed(A_START)) {
        nextLevel();
        return;
    }

    if (gameOver_) {
        gameOverTimer_ -= dt;
        if (gameOverTimer_ <= 0.0f) {
            lives_ = 3;
            score_ = 0;
            loadLevelIndex(0);
        }
        return;
    }

    if (levelWon_) {
        winTimer_ -= dt;
        if (winTimer_ <= 0.0f) {
            nextLevel();
        }
        return;
    }

    updatePlayer(dt);
    updateEntities(dt);
    checkCollisions();

    // Camera tracks player
    camX_ = std::clamp(int(px_) - LOGICAL_W / 2, 0, std::max(0, level_.width * TILE_SIZE - LOGICAL_W));
    camY_ = 0;
}

void Game::renderBackground(SDL_Renderer* ren) {
    if (texBg_) {
        // Scrolling parallax background
        const int bgW = 512;
        const int offset = (camX_ / 2) % bgW;
        for (int x = -offset; x < LOGICAL_W; x += bgW) {
            SDL_Rect dst{x, 0, bgW, LOGICAL_H};
            SDL_RenderCopy(ren, texBg_, nullptr, &dst);
        }
    } else {
        SDL_SetRenderDrawColor(ren, 30, 40, 60, 255);
        SDL_RenderClear(ren);
    }
}

void Game::renderTilemap(SDL_Renderer* ren) {
    const int startX = std::max(0, camX_ / TILE_SIZE);
    const int endX   = std::min(level_.width, (camX_ + LOGICAL_W) / TILE_SIZE + 1);

    for (int x = startX; x < endX; ++x) {
        for (int y = 0; y < kLevelRows; ++y) {
            uint32_t tid = level_.at(x, y);
            if (tid == 0) continue;

            SDL_Rect dst{x * TILE_SIZE - camX_, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

            if (texTiles_ && tid < 64) {
                // Authentic 8x8 grid of 64x64 tiles on 512x512 texture
                const int row = int(tid) / 8;
                const int col = int(tid) % 8;
                SDL_Rect src{col * SHEET_TILE_PX, row * SHEET_TILE_PX, SHEET_TILE_PX, SHEET_TILE_PX};
                SDL_RenderCopy(ren, texTiles_, &src, &dst);
            } else {
                // Fallback colored tile
                SDL_SetRenderDrawColor(ren, Uint8((tid * 53) % 200 + 40),
                                            Uint8((tid * 97) % 200 + 40),
                                            Uint8((tid * 29) % 200 + 40), 255);
                SDL_RenderFillRect(ren, &dst);
            }
        }
    }
}

void Game::renderEntities(SDL_Renderer* ren) {
    for (const auto& e : entities_) {
        if (!e.active) continue;

        SDL_Rect dst{int(e.x) - camX_, int(e.y), e.width, e.height};

        if (e.type == ENT_COIN && texCoins_) {
            // Coins animated frame (128x128 sheet, frames 0..3)
            SDL_Rect src{e.animFrame * 32, 0, 32, 32};
            SDL_RenderCopy(ren, texCoins_, &src, &dst);
        } else if (e.type == ENT_CHECKPOINT && texCheckpoint_) {
            SDL_RenderCopy(ren, texCheckpoint_, nullptr, &dst);
        } else if (e.type == ENT_EXIT && texSigns_) {
            SDL_RenderCopy(ren, texSigns_, nullptr, &dst);
        } else if ((e.type == ENT_ENEMY_SPIKEY || e.type == ENT_ENEMY_BLOB) && texEnemies1_) {
            SDL_RendererFlip flip = (e.vx > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(ren, texEnemies1_, nullptr, &dst, 0.0, nullptr, flip);
        } else {
            // Colored placeholder
            if (e.type == ENT_COIN) SDL_SetRenderDrawColor(ren, 255, 220, 0, 255);
            else if (e.type == ENT_CHECKPOINT) SDL_SetRenderDrawColor(ren, 50, 180, 255, 255);
            else if (e.type == ENT_EXIT) SDL_SetRenderDrawColor(ren, 80, 255, 80, 255);
            else SDL_SetRenderDrawColor(ren, 255, 60, 60, 255);
            SDL_RenderFillRect(ren, &dst);
        }
    }
}

void Game::renderPlayer(SDL_Renderer* ren) {
    // Flash if invulnerable
    if (invulnerableTimer_ > 0.0f && (int(invulnerableTimer_ * 10) % 2 == 0)) {
        return;
    }

    SDL_Rect dst{int(px_) - camX_, int(py_), 32, 32};

    if (texPlayer_) {
        SDL_RendererFlip flip = facingRight_ ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(ren, texPlayer_, nullptr, &dst, 0.0, nullptr, flip);
    } else {
        SDL_SetRenderDrawColor(ren, 255, 230, 40, 255);
        SDL_RenderFillRect(ren, &dst);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawRect(ren, &dst);
    }
}

void Game::renderHUD(SDL_Renderer* ren) {
    // Lives display
    for (int i = 0; i < lives_; ++i) {
        SDL_Rect r{16 + i * 28, 16, 24, 24};
        if (texLife_) {
            SDL_RenderCopy(ren, texLife_, nullptr, &r);
        } else {
            SDL_SetRenderDrawColor(ren, 255, 50, 50, 255);
            SDL_RenderFillRect(ren, &r);
        }
    }

    // Score display boxes
    SDL_Rect scoreBox{LOGICAL_W / 2 - 60, 16, 120, 24};
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 140);
    SDL_RenderFillRect(ren, &scoreBox);

    // Victory or Game Over banner
    if (levelWon_ && texVictory_) {
        SDL_Rect vr{LOGICAL_W / 2 - 200, LOGICAL_H / 2 - 150, 400, 300};
        SDL_RenderCopy(ren, texVictory_, nullptr, &vr);
    } else if (gameOver_) {
        SDL_Rect gor{LOGICAL_W / 2 - 150, LOGICAL_H / 2 - 40, 300, 80};
        SDL_SetRenderDrawColor(ren, 200, 20, 20, 220);
        SDL_RenderFillRect(ren, &gor);
    }
}

void Game::render(SDL_Renderer* ren) {
    renderBackground(ren);
    renderTilemap(ren);
    renderEntities(ren);
    renderPlayer(ren);
    renderHUD(ren);

    // Render Touch Overlay (auto-visible on touch devices or toggled on desktop)
    input_.drawOverlay(ren, LOGICAL_W, LOGICAL_H);

    SDL_RenderPresent(ren);
}
