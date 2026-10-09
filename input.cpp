// Input Emulation Subsystem Implementation
#include "input.hpp"
#include <cstring>
#include <algorithm>

Input::Input()
    : pad_(nullptr), joy_(nullptr),
      mouseAsTouch_(false), usedTouch_(false),
      showOverlay_(false), stickDeadzone_(8000) {

    for (int i = 0; i < A_COUNT; ++i) {
        cur_[i] = false;
        prev_[i] = false;
    }
    mouseTouch_ = {-1.0f, -1.0f};

    // Auto-enable touch overlay on mobile & handheld touch devices
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE) || defined(__IPHONEOS__) || \
    defined(_WIN32_WCE) || defined(__SYMBIAN32__) || defined(__3DS__) || defined(__vita__)
    showOverlay_ = true;
    usedTouch_ = true;
#endif

    // Open first available controller if connected
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            pad_ = SDL_GameControllerOpen(i);
            if (pad_) break;
        } else if (!joy_) {
            joy_ = SDL_JoystickOpen(i);
        }
    }

    initTouchZones();
}

Input::~Input() {
    if (pad_) SDL_GameControllerClose(pad_);
    if (joy_) SDL_JoystickClose(joy_);
}

void Input::initTouchZones() {
    zones_.clear();

    // D-Pad (Bottom-Left)
    // Left: (0.02, 0.72, 0.11, 0.22)
    zones_.push_back({0.02f, 0.72f, 0.11f, 0.22f, A_LEFT, "<", false});
    // Right: (0.16, 0.72, 0.11, 0.22)
    zones_.push_back({0.16f, 0.72f, 0.11f, 0.22f, A_RIGHT, ">", false});
    // Up: (0.09f, 0.52f, 0.11f, 0.20f)
    zones_.push_back({0.09f, 0.52f, 0.11f, 0.20f, A_UP, "^", false});
    // Down: (0.09f, 0.82f, 0.11f, 0.16f)
    zones_.push_back({0.09f, 0.82f, 0.11f, 0.16f, A_DOWN, "v", false});

    // Action Buttons (Bottom-Right)
    // Jump Button (A): (0.76, 0.70, 0.18, 0.24) - Large circular button
    zones_.push_back({0.76f, 0.70f, 0.18f, 0.24f, A_JUMP, "JUMP", true});
    // Action/Fire Button (B): (0.62, 0.74, 0.12, 0.18)
    zones_.push_back({0.62f, 0.74f, 0.12f, 0.18f, A_ACTION, "ACT", true});

    // Top Controls
    // Menu / Back (Top-Left): (0.02, 0.02, 0.10, 0.08)
    zones_.push_back({0.02f, 0.02f, 0.10f, 0.08f, A_BACK, "BACK", false});
    // Start / Pause (Top-Right): (0.88, 0.02, 0.10, 0.08)
    zones_.push_back({0.88f, 0.02f, 0.10f, 0.08f, A_START, "START", false});
}

void Input::handleEvent(const SDL_Event& e) {
    switch (e.type) {
    // Controller hotplugging
    case SDL_CONTROLLERDEVICEADDED:
        if (!pad_) {
            pad_ = SDL_GameControllerOpen(e.cdevice.which);
        }
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        if (pad_ && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad_)) == e.cdevice.which) {
            SDL_GameControllerClose(pad_);
            pad_ = nullptr;
        }
        break;

    // Joystick hotplugging (fallback for non-standard controllers)
    case SDL_JOYDEVICEADDED:
        if (!pad_ && !joy_) {
            joy_ = SDL_JoystickOpen(e.jdevice.which);
        }
        break;
    case SDL_JOYDEVICEREMOVED:
        if (joy_ && SDL_JoystickInstanceID(joy_) == e.jdevice.which) {
            SDL_JoystickClose(joy_);
            joy_ = nullptr;
        }
        break;

    // Multi-touch events
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
        fingers_[e.tfinger.fingerId] = {e.tfinger.x, e.tfinger.y};
        usedTouch_ = true;
        showOverlay_ = true;
        break;
    case SDL_FINGERUP:
        fingers_.erase(e.tfinger.fingerId);
        break;

    // Mouse-as-touch emulation (useful on desktop or single-touch devices)
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
            int w, h;
            SDL_GetWindowSize(SDL_GetWindowFromID(e.button.windowID), &w, &h);
            if (w > 0 && h > 0) {
                mouseTouch_ = {float(e.button.x) / float(w), float(e.button.y) / float(h)};
                mouseAsTouch_ = true;
            }
        }
        break;
    case SDL_MOUSEMOTION:
        if (mouseAsTouch_) {
            int w, h;
            SDL_GetWindowSize(SDL_GetWindowFromID(e.motion.windowID), &w, &h);
            if (w > 0 && h > 0) {
                mouseTouch_ = {float(e.motion.x) / float(w), float(e.motion.y) / float(h)};
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT) {
            mouseAsTouch_ = false;
            mouseTouch_ = {-1.0f, -1.0f};
        }
        break;

    // Toggle overlay with F1 or backquote
    case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_F1 || e.key.keysym.sym == SDLK_TAB) {
            toggleTouchOverlay();
        }
        break;
    }
}

void Input::updateKeyboard() {
    const Uint8* k = SDL_GetKeyboardState(nullptr);
    if (!k) return;

    // Movement: Arrows or WASD
    cur_[A_LEFT]  |= (k[SDL_SCANCODE_LEFT]  || k[SDL_SCANCODE_A]);
    cur_[A_RIGHT] |= (k[SDL_SCANCODE_RIGHT] || k[SDL_SCANCODE_D]);
    cur_[A_UP]    |= (k[SDL_SCANCODE_UP]    || k[SDL_SCANCODE_W]);
    cur_[A_DOWN]  |= (k[SDL_SCANCODE_DOWN]  || k[SDL_SCANCODE_S]);

    // Jump: Space, Z, or Keypad 0
    cur_[A_JUMP]  |= (k[SDL_SCANCODE_SPACE] || k[SDL_SCANCODE_Z] || k[SDL_SCANCODE_KP_0]);

    // Action/Run: X, C, or Left Shift
    cur_[A_ACTION] |= (k[SDL_SCANCODE_X] || k[SDL_SCANCODE_C] || k[SDL_SCANCODE_LSHIFT]);

    // Start / Pause: Enter or Return or P
    cur_[A_START] |= (k[SDL_SCANCODE_RETURN] || k[SDL_SCANCODE_RETURN2] || k[SDL_SCANCODE_P]);

    // Back / Menu: Escape, Backspace, or Q
    cur_[A_BACK]  |= (k[SDL_SCANCODE_ESCAPE] || k[SDL_SCANCODE_BACKSPACE] || k[SDL_SCANCODE_Q]);

    // Select: Right Shift or Tab
    cur_[A_SELECT] |= (k[SDL_SCANCODE_RSHIFT] || k[SDL_SCANCODE_GRAVE]);
}

void Input::updateGamepad() {
    if (pad_) {
        // D-Pad
        cur_[A_LEFT]  |= SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0;
        cur_[A_RIGHT] |= SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0;
        cur_[A_UP]    |= SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0;
        cur_[A_DOWN]  |= SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0;

        // Analog Left Stick with deadzone
        const Sint16 ax = SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTX);
        const Sint16 ay = SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTY);
        if (ax < -stickDeadzone_) cur_[A_LEFT]  = true;
        if (ax >  stickDeadzone_) cur_[A_RIGHT] = true;
        if (ay < -stickDeadzone_) cur_[A_UP]    = true;
        if (ay >  stickDeadzone_) cur_[A_DOWN]  = true;

        // Action Buttons:
        // A/B (Xbox A/B, PS Cross/Circle, Nintendo B/A) -> Jump
        cur_[A_JUMP]   |= (SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_A) != 0 ||
                           SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_B) != 0);

        // X/Y (Xbox X/Y, PS Square/Triangle, Nintendo Y/X) -> Action
        cur_[A_ACTION] |= (SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_X) != 0 ||
                           SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_Y) != 0);

        // Start / Pause
        cur_[A_START]  |= (SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_START) != 0);

        // Back / Cancel
        cur_[A_BACK]   |= (SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_BACK) != 0 ||
                           SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_GUIDE) != 0);
    } else if (joy_) {
        // Fallback for standard joystick / retro consoles
        const int numAxes = SDL_JoystickNumAxes(joy_);
        if (numAxes >= 2) {
            const Sint16 ax = SDL_JoystickGetAxis(joy_, 0);
            const Sint16 ay = SDL_JoystickGetAxis(joy_, 1);
            if (ax < -stickDeadzone_) cur_[A_LEFT]  = true;
            if (ax >  stickDeadzone_) cur_[A_RIGHT] = true;
            if (ay < -stickDeadzone_) cur_[A_UP]    = true;
            if (ay >  stickDeadzone_) cur_[A_DOWN]  = true;
        }
        // Check hats (D-pad on many joysticks)
        if (SDL_JoystickNumHats(joy_) > 0) {
            Uint8 hat = SDL_JoystickGetHat(joy_, 0);
            if (hat & SDL_HAT_LEFT)  cur_[A_LEFT]  = true;
            if (hat & SDL_HAT_RIGHT) cur_[A_RIGHT] = true;
            if (hat & SDL_HAT_UP)    cur_[A_UP]    = true;
            if (hat & SDL_HAT_DOWN)  cur_[A_DOWN]  = true;
        }
        // Buttons
        const int numButtons = SDL_JoystickNumButtons(joy_);
        if (numButtons > 0) cur_[A_JUMP]   |= (SDL_JoystickGetButton(joy_, 0) != 0);
        if (numButtons > 1) cur_[A_ACTION] |= (SDL_JoystickGetButton(joy_, 1) != 0);
        if (numButtons > 2) cur_[A_BACK]   |= (SDL_JoystickGetButton(joy_, 2) != 0);
        if (numButtons > 7) cur_[A_START]  |= (SDL_JoystickGetButton(joy_, 7) != 0);
    }
}

void Input::updateTouch() {
    auto checkPoint = [&](float px, float py) {
        for (const auto& z : zones_) {
            if (px >= z.x && px <= z.x + z.w && py >= z.y && py <= z.y + z.h) {
                cur_[z.action] = true;
            }
        }
    };

    // Fingers
    for (const auto& pair : fingers_) {
        checkPoint(pair.second.x, pair.second.y);
    }

    // Mouse emulation
    if (mouseAsTouch_) {
        checkPoint(mouseTouch_.x, mouseTouch_.y);
    }
}

void Input::update() {
    // Shift current to previous
    for (int i = 0; i < A_COUNT; ++i) {
        prev_[i] = cur_[i];
        cur_[i] = false;
    }

    // Accumulate inputs across all subsystems
    updateKeyboard();
    updateGamepad();
    updateTouch();
}

void Input::drawOverlay(SDL_Renderer* renderer, int screenW, int screenH) const {
    if (!showOverlay_ || !renderer) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (const auto& z : zones_) {
        const int rx = int(z.x * screenW);
        const int ry = int(z.y * screenH);
        const int rw = int(z.w * screenW);
        const int rh = int(z.h * screenH);

        const bool isActive = cur_[z.action];

        // Background color
        if (isActive) {
            // Highlighted active state: bright golden/white glow
            SDL_SetRenderDrawColor(renderer, 240, 200, 80, 160);
        } else {
            // Idle state: subtle translucent grey
            SDL_SetRenderDrawColor(renderer, 60, 70, 90, 85);
        }

        SDL_Rect rect{rx, ry, rw, rh};
        SDL_RenderFillRect(renderer, &rect);

        // Border outline
        if (isActive) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 240);
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 190, 210, 130);
        }
        SDL_RenderDrawRect(renderer, &rect);

        // Inner accent border for depth
        SDL_Rect innerRect{rx + 2, ry + 2, rw - 4, rh - 4};
        if (innerRect.w > 0 && innerRect.h > 0) {
            SDL_RenderDrawRect(renderer, &innerRect);
        }
    }
}
