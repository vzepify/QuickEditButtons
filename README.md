# QuickEditButtons — Geode Mod

Adds **Flip X**, **Flip Y**, **Rotate CW**, and **Rotate CCW** buttons directly
onto the **first page of the Edit tab** in the Geometry Dash level editor.
No more flipping through pages — the four most-used transform buttons are
always in reach.

---

## Compatibility

| Platform | GD Version | Geode SDK |
|----------|-----------|-----------|
| Android  | 2.2074    | ≥ 4.0.0   |

---

## How to Build (Android)

### Prerequisites

1. **Geode SDK** — clone and set the `GEODE_SDK` environment variable:
   ```bash
   git clone https://github.com/geode-sdk/geode.git ~/geode-sdk
   export GEODE_SDK=~/geode-sdk          # add to ~/.bashrc / ~/.zshrc
   ```

2. **Android NDK r26b** (or later) and **CMake 3.21+**:
   ```bash
   # Via Android Studio SDK Manager, or directly:
   sdkmanager "ndk;26.1.10909125" "cmake;3.22.1"
   ```

3. **Geode CLI** (optional but recommended for packaging):
   ```bash
   # https://github.com/geode-sdk/cli/releases
   # Put the binary on your PATH
   ```

### Build steps

```bash
# 1. Clone / copy this project
cd QuickEditButtons

# 2. Configure for Android ARM64
cmake -B build-android \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-23

# 3. Compile
cmake --build build-android --config Release

# 4. Package into a .geode file  (requires Geode CLI)
geode package new build-android/QuickEditButtons.so \
  --binary build-android/QuickEditButtons.so \
  --output QuickEditButtons.geode
```

The output is `QuickEditButtons.geode`.

---

## Installation (Android)

### Via Geode Launcher (recommended)

1. Install **Geode** on your Android device:
   https://github.com/geode-sdk/geode/releases
2. Copy `QuickEditButtons.geode` to:
   ```
   /sdcard/Android/data/com.robtopx.geometryjump/files/geode/mods/
   ```
3. Launch Geometry Dash — the mod loads automatically.

### Via Geode in-game installer

If you publish the `.geode` file to the Geode index, players can install it
directly from the in-game Mods menu.

---

## What it does

```
Edit tab  →  Page 1  (before)          Edit tab  →  Page 1  (after)
┌─────────────────────────┐            ┌─────────────────────────┐
│  [Move] [Scale] [Rot]…  │            │ [FlipX][FlipY][CW][CCW] │  ← new row
│  …lots of buttons…      │            │  [Move] [Scale] [Rot]…  │
└─────────────────────────┘            └─────────────────────────┘
```

The four buttons use the same sprites GD already ships, so they look
completely native:

| Button | Sprite frame            | Action                  |
|--------|------------------------|-------------------------|
| ↔      | edit_flipXBtn_001.png  | Flip selected objects X |
| ↕      | edit_flipYBtn_001.png  | Flip selected objects Y |
| ↻      | edit_cwBtn_001.png     | Rotate 90° clockwise    |
| ↺      | edit_ccwBtn_001.png    | Rotate 90° counter-CW   |

---

## Project structure

```
QuickEditButtons/
├── CMakeLists.txt   — build configuration
├── mod.json         — Geode metadata
├── README.md        — this file
└── src/
    └── main.cpp     — hook into EditorUI, inject buttons
```

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| Buttons don't appear | Make sure GD version matches `2.2074` in mod.json |
| Build error `GEODE_SDK not found` | Export `GEODE_SDK` env var pointing to cloned SDK |
| `.geode` file not loading | Confirm Geode launcher is installed and up to date |
| Buttons overlap existing UI | Open an issue — layout offsets may differ on your screen resolution |

---

## License

MIT — do whatever you want with this mod.
