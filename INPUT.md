# Input Emulation Subsystem

The input system in Cadog Adventures translates disparate input mechanisms—desktop keyboards, modern gamepads, retro console controllers, and multi-touch mobile screens—into a unified, abstract set of actions:

```
[Keyboard / Mice] ──┐
[Gamepads / Joysticks] ──┼──> [ Input Emulation Engine ] ──> [ Game Logic (A_LEFT, A_JUMP, etc.) ]
[Multi-Touch Overlay] ──┘
```

The game code queries only abstract actions (`A_LEFT`, `A_RIGHT`, `A_UP`, `A_DOWN`, `A_JUMP`, `A_ACTION`, `A_START`, `A_BACK`), remaining completely agnostic of the physical hardware in use.

---

## Action Mapping Matrix

| Action | Keyboard | Modern Gamepad | Xbox 360 | PS3 / PS Vita | Nintendo Wii | Nintendo 3DS | Mobile / Touch Screen |
|--------|----------|----------------|----------|---------------|--------------|--------------|-----------------------|
| **Move Left** | Left Arrow / `A` | D-Pad Left / Stick Left | D-Pad Left / L-Stick | D-Pad Left / L-Stick | Wiimote Left / Stick | Circle Pad / D-Pad Left | Virtual `<` button |
| **Move Right**| Right Arrow / `D`| D-Pad Right / Stick Right| D-Pad Right / L-Stick| D-Pad Right / L-Stick| Wiimote Right / Stick| Circle Pad / D-Pad Right| Virtual `>` button |
| **Look Up**   | Up Arrow / `W`   | D-Pad Up / Stick Up     | D-Pad Up / L-Stick   | D-Pad Up / L-Stick   | Wiimote Up / Stick   | Circle Pad / D-Pad Up   | Virtual `^` button |
| **Look Down** | Down Arrow / `S` | D-Pad Down / Stick Down | D-Pad Down / L-Stick | D-Pad Down / L-Stick | Wiimote Down / Stick | Circle Pad / D-Pad Down | Virtual `v` button |
| **Jump**      | Space / `Z`      | Button A / B            | Button `A`           | Button Cross (`X`)   | Wiimote `2` / Button `A` | Button `A` / `B`    | Large `JUMP` button |
| **Action**    | `X` / `C` / Shift| Button X / Y            | Button `X`           | Button Square        | Wiimote `1` / Button `B` | Button `X` / `Y`    | Virtual `ACT` button |
| **Start/Next**| Enter / Return   | Start                   | Start                | Start                | Wiimote `+`          | Start                   | Top-right `START` |
| **Back/Menu** | Escape / Backspace| Back / Guide           | Back                 | Select               | Wiimote `-` / Home   | Select                  | Top-left `BACK` |

---

## Virtual Touch Screen Emulation

On mobile platforms (**Android**, **iOS**, **Windows Phone**, **Symbian Touch**), as well as touchscreen consoles (**Nintendo 3DS** bottom screen, **PS Vita** front OLED touch):

1. **Auto-Detection:**
   - The on-screen touch overlay is automatically enabled whenever running on mobile operating systems, or when the first touch event (`SDL_FINGERDOWN`) is registered.
   - On desktop computers, developers can toggle the touch overlay anytime using `Tab` or `F1`, and interact with the virtual controls via mouse clicks.

2. **Overlay Layout:**
   - **Bottom-Left:** Ergonomic 4-way D-Pad (`<`, `>`, `^`, `v`) with generous touch targets.
   - **Bottom-Right:** Prominent circular `JUMP` button and adjacent `ACT` button.
   - **Top-Bar:** Quick-access `BACK` and `START / PAUSE` buttons.

3. **Visual Feedback:**
   - High-contrast, semi-transparent HUD design that keeps the gameplay area visible.
   - Dynamic illumination: pressed buttons brighten to a tactile golden-white glow with double-border depth.

---

## Gamepad & Joystick Emulation

- **Hotplug Support:** Seamlessly handles controllers plugged in or disconnected at runtime via `SDL_CONTROLLERDEVICEADDED` / `SDL_CONTROLLERDEVICEREMOVED`.
- **Analog Deadzone Filtering:** Analog sticks feature an 8000-unit deadzone threshold to eliminate stick drift while maintaining immediate responsiveness.
- **Legacy Joystick Fallback:** For consoles without standard XInput/SDL GameController mappings (such as Wii or homebrew controllers), the engine automatically falls back to raw `SDL_Joystick` axes and button masks.
