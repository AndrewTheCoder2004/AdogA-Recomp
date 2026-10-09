#pragma once
// Input Emulation Subsystem for Cadog Adventures
// Unifies Keyboard, Gamepad (Modern + Console), and Virtual Touch Overlay
// into an abstract action interface for all 16 target platforms.

#include <SDL.h>
#include <map>
#include <vector>
#include <string>
#include <cmath>

// Abstract game actions
enum Action {
    A_LEFT = 0,
    A_RIGHT,
    A_UP,
    A_DOWN,
    A_JUMP,
    A_ACTION,
    A_START,
    A_BACK,
    A_SELECT,
    A_COUNT
};

// Input modes / hardware profiles
enum InputProfile {
    PROFILE_AUTO = 0,
    PROFILE_DESKTOP_KEYBOARD,
    PROFILE_GAMEPAD_STANDARD,
    PROFILE_CONSOLE_WII,
    PROFILE_CONSOLE_3DS,
    PROFILE_CONSOLE_VITA,
    PROFILE_CONSOLE_PS3,
    PROFILE_CONSOLE_XBOX360,
    PROFILE_MOBILE_TOUCH
};

class Input {
public:
    Input();
    ~Input();

    // Query action state
    bool isDown(Action a) const { return (a >= 0 && a < A_COUNT) ? cur_[a] : false; }
    bool isJustPressed(Action a) const { return (a >= 0 && a < A_COUNT) ? (cur_[a] && !prev_[a]) : false; }
    bool isJustReleased(Action a) const { return (a >= 0 && a < A_COUNT) ? (!cur_[a] && prev_[a]) : false; }

    // Convenience shorthand
    bool down(Action a) const { return isDown(a); }
    bool pressed(Action a) const { return isJustPressed(a); }
    bool released(Action a) const { return isJustReleased(a); }

    // Process SDL event (call inside SDL_PollEvent loop)
    void handleEvent(const SDL_Event& e);

    // Update state (call once per frame after event loop)
    void update();

    // Render virtual touch overlay on screen
    void drawOverlay(SDL_Renderer* renderer, int screenW, int screenH) const;

    // Touch overlay control
    void setTouchOverlayEnabled(bool enabled) { showOverlay_ = enabled; }
    bool isTouchOverlayEnabled() const { return showOverlay_; }
    void toggleTouchOverlay() { showOverlay_ = !showOverlay_; }

    // Deadzone configuration for analog sticks
    void setStickDeadzone(int deadzone) { stickDeadzone_ = deadzone; }
    int getStickDeadzone() const { return stickDeadzone_; }

    // Check currently detected hardware
    bool hasGamepad() const { return pad_ != nullptr || joy_ != nullptr; }
    bool hasTouchInput() const { return usedTouch_; }

private:
    struct TouchZone {
        float x, y, w, h;       // Normalized coordinates (0.0 to 1.0)
        Action action;
        const char* label;
        bool isCircle;
    };

    struct TouchPoint {
        float x;
        float y;
    };

    void initTouchZones();
    void updateKeyboard();
    void updateGamepad();
    void updateTouch();

    bool cur_[A_COUNT];
    bool prev_[A_COUNT];

    // Hardware handles
    SDL_GameController* pad_;
    SDL_Joystick* joy_;

    // Touch tracking
    std::map<SDL_FingerID, TouchPoint> fingers_;
    TouchPoint mouseTouch_;
    bool mouseAsTouch_;
    bool usedTouch_;
    bool showOverlay_;

    // Settings
    int stickDeadzone_;
    std::vector<TouchZone> zones_;
};
