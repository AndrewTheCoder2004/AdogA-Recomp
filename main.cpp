// Cadog Adventures - Modern Cross-Platform Recompilation
// Supports: Win10 x86, Win11 x64, Linux, BSD, macOS, iOS, Windows Phone, Android,
//           HaikuOS, ArcaOS, Wii, Xbox 360, PS3, 3DS, PS Vita, and Symbian.
#include <SDL.h>
#include <cstdio>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "game.hpp"

static bool dirExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

static std::string resolveAssetsPath(int argc, char** argv) {
    if (argc > 1 && dirExists(argv[1])) {
        return argv[1];
    }

    std::vector<std::string> candidates = {
        "assets",
        "../assets",
        "../../assets",
        "assets/CadogAdventures",
        // System paths
        "/usr/share/cadog/assets",
        "/usr/local/share/cadog/assets"
    };

    // Platform-specific defaults
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        std::string base(basePath);
        candidates.push_back(base + "assets");
        candidates.push_back(base + "../assets");
        candidates.push_back(base + "CadogAdventures/assets");
        SDL_free(basePath);
    }

    char* prefPath = SDL_GetPrefPath("", "CadogAdventures");
    if (prefPath) {
        std::string pref(prefPath);
        candidates.push_back(pref + "assets");
        SDL_free(prefPath);
    }

    for (const auto& path : candidates) {
        if (dirExists(path)) {
            return path;
        }
    }

    return "assets"; // Default fallback
}

int main(int argc, char** argv) {
    // Initialize SDL2 subsystems
    Uint32 initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK;
    if (SDL_Init(initFlags) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    const int WIN_WIDTH = 800;
    const int WIN_HEIGHT = 600;

    Uint32 winFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE) || defined(__IPHONEOS__) || \
    defined(__3DS__) || defined(__vita__) || defined(_WIN32_WCE)
    winFlags |= SDL_WINDOW_FULLSCREEN;
#endif

    SDL_Window* window = SDL_CreateWindow(
        "Cadog Adventures",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        winFlags
    );

    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        // Fallback to software renderer on retro/embedded platforms
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    SDL_RenderSetLogicalSize(renderer, WIN_WIDTH, WIN_HEIGHT);

    std::string assetsPath = resolveAssetsPath(argc, argv);
    std::printf("Using assets directory: %s\n", assetsPath.c_str());

    Game game;
    if (!game.init(renderer, assetsPath)) {
        std::fprintf(stderr, "Warning: Assets not fully loaded from %s. Run launcher.py to extract.\n", assetsPath.c_str());
    }

    Uint32 lastTime = SDL_GetTicks();

    // Main Game Loop
    while (game.isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            game.handleEvent(event);
        }

        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Cap dt to prevent tunneling on lag spikes
        if (dt > 0.05f) dt = 0.05f;

        game.update(dt);
        game.render(renderer);

        // Frame cap ~60 FPS
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
