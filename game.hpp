#pragma once
// Game Engine and Entity System for Cadog Adventures
// Features:
// - Authentic tilemap rendering from extracted TGA sheets (8x8 grid of 64x64 tiles)
// - Entity management (Coins, Enemies, Signs, Checkpoints)
// - Player platformer physics with tile collisions
// - Parallax background scrolling
// - HUD with score, lives, and level progression
// - Integrated Audio and Input Emulation Subsystems

#include <SDL.h>
#include <string>
#include <vector>
#include "formats.hpp"
#include "input.hpp"
#include "audio.hpp"

// Entity types
enum EntityType {
    ENT_NONE = 0,
    ENT_COIN,
    ENT_CHECKPOINT,
    ENT_EXIT,
    ENT_ENEMY_SPIKEY,
    ENT_ENEMY_BLOB,
    ENT_ENEMY_COCOA,
    ENT_ENEMY_PARROT
};

struct Entity {
    EntityType type = ENT_NONE;
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    float animTimer = 0;
    int animFrame = 0;
    bool active = true;
    bool triggered = false;
    int width = 32;
    int height = 32;
};

class Game {
public:
    Game();
    ~Game();

    bool init(SDL_Renderer* renderer, const std::string& assetsPath);
    void handleEvent(const SDL_Event& e);
    void update(float dt);
    void render(SDL_Renderer* renderer);
    bool isRunning() const { return running_; }

    void loadLevelIndex(int index);
    void nextLevel();
    void restartLevel();

private:
    void initEntitiesFromMap();
    void updatePlayer(float dt);
    void updateEntities(float dt);
    void checkCollisions();
    bool isSolidTile(int tx, int ty) const;

    // Rendering helpers
    void renderBackground(SDL_Renderer* renderer);
    void renderTilemap(SDL_Renderer* renderer);
    void renderEntities(SDL_Renderer* renderer);
    void renderPlayer(SDL_Renderer* renderer);
    void renderHUD(SDL_Renderer* renderer);

    std::string assetsPath_;
    bool running_;

    // Assets / Textures
    SDL_Texture* texBg_ = nullptr;
    SDL_Texture* texTiles_ = nullptr;
    SDL_Texture* texPlayer_ = nullptr;
    SDL_Texture* texPlayer2_ = nullptr;
    SDL_Texture* texCoins_ = nullptr;
    SDL_Texture* texSigns_ = nullptr;
    SDL_Texture* texCheckpoint_ = nullptr;
    SDL_Texture* texEnemies1_ = nullptr;
    SDL_Texture* texEnemies2_ = nullptr;
    SDL_Texture* texLife_ = nullptr;
    SDL_Texture* texNumbers_ = nullptr;
    SDL_Texture* texVictory_ = nullptr;

    // Subsystems
    Input input_;
    Audio audio_;

    // Level state
    int currentLevelIdx_;
    Level level_;
    std::vector<Entity> entities_;

    // Player state
    float px_, py_;
    float pvx_, pvy_;
    bool onGround_;
    bool facingRight_;
    int lives_;
    int score_;
    float invulnerableTimer_;
    float spawnX_, spawnY_;

    // Camera
    int camX_, camY_;

    // Game stats & state
    bool levelWon_;
    float winTimer_;
    bool gameOver_;
    float gameOverTimer_;

    static const int LOGICAL_W = 800;
    static const int LOGICAL_H = 600;
    static const int TILE_SIZE = 32;
    static const int SHEET_TILE_PX = 64;
};
