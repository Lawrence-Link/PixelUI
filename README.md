# PixelUI

A lightweight **C++ UI framework** for resource-constrained embedded devices (e.g. ESP32).  
PixelUI provides a **modular, event-driven, and component-based** UI system with smooth rendering and animations powered by [U8G2](https://github.com/olikraus/u8g2).  
Its design focuses on **performance, low memory footprint, and easy extensibility**, enabling developers to build complex UIs with minimal effort.

---

![AppView Interface](doc/img/Animation.gif)

## ✨ Features

- 🎛 **Core Scheduler**: Event-driven architecture with `Heartbeat()` for logic & animation updates and `renderer()` for drawing.
- 🎞 **Animation Engine**: Fixed-point easing curves, protected animations, and centralized management via `AnimationManager`.
- 📚 **View & Input Management**: Stack-based navigation, popup routing, and strict input priority handling.
- 🧩 **Component-Based Widgets**: Unified `Widget` base class for reusable UI elements (`Brace`, `Histogram`, etc.).
- 📜 **ListView**: Smooth scrolling menus with sub-items, actions, and configurable options.
- 🚀 **Resource Optimized**: Minimal heap usage, separate rendering & logic loops, and stable long-term operation.

---

## 🏗 Architecture

### Core
- **PixelUI**: Entry point and central dispatcher, including input routing and animations, etc.
- **Renderer**: Draws the current UI to display buffer.
- **AnimationManager** manages animations with shared pointers. using Fixed-point arithmetic ensures predictable performance on MCUs without FPU. Supports protected animations, survive bulk cleanup operations.
- **Coroutine** A light-weight coroutine implementation, useful when writing animation load sequence.
- **ViewManager**: Stack-based view management.

### Components
- **Widget** versatile widget system, supports custom widgets with animations and interactions.
- **ListView**: Scrollable menu supporting:
  - Submenus
  - Executable items
  - Configurable boolean/integer options
- **IconView**: Scrollable menu, displays item icon for selection.
- **AppLauncher**: Implementation of IconView, coupled with AppSystem to acquire all registered apps.
- **Popup**: PopupManagers, Popup base and interface, Popup derivations
- **Focus**: Focus system for widget choosing.
- **Blinker**: Non-blocking blink indicator.

### Resource Strategy
- Minimized dynamic memory allocation to avoid fragmentation.
- Logic (`Heartbeat`) and rendering (`renderer`) fully separated.

## 📦 Getting Started

#Third-Party Dependencies (Git Submodules)
- This project uses the following Git submodules, placed under **third_party/**:
    - ETL (Embedded Template Library)
    - U8G2

Initialize/update submodules after clone to download them under **third_party/** :

```bash
git submodule update --init --recursive
```

# Build simulator
- To build the PC Qt6 simulator, ensure the following option exists in the root CMakeLists.txt (default ON):
```bash
option(BUILD_SIMULATOR "Build PC Qt simulator" ON)
```

- You can also set it via CMake command line:
```bash
cmake -B build -S . -DBUILD_SIMULATOR=ON
cmake --build build
```
# Basic steps of adapting it to your system
- Port U8g2lib, add both csrc and cppsrc folders in your include directory as the library include its headers directly
- Import ETL (Embedded Template Libraries), also add the include directory
 as the library include its headers under etl/*.h

```cpp
#include <U8g2lib.h>
#include "PixelUI.h"

U8G2 u8g2(...);
PixelUI ui(u8g2);

// Update logic (called every ~10ms, e.g. from a timer/RTOS task)
// This provides all task managing.

void TimerElapsedISR() { // assuming trigger every 10ms, provide ~ 100FPS frame rate at most.
    ui.Heartbeat(10);
}

int main() {
    // Create your main app, or task, mainly reserved for ui rendering and input event receiving.
    u8g2.begin();

    #if (USE_STATIC_APP_REGISTER_ENABLED == 0) // If not using static app linking, register Apps manually
    registerApps();
    #endif

    ui.begin(); // initialize UI instance

    auto appView = AppLauncher::createAppLauncherView(ui, *ui.getViewManagerPtr()); // appView to show loaded apps

    ui.getViewManager().push(appView); // push in the appView to the ViewStack

    while(true) {
        // handleInput from the queue or something else
        if (auto event = readInput()) {
            ui.handleInput(event.value());
        }
        // Render UI (can be lower priority task)
        ui.renderer();
    }
}
```

## 📜 License

PixelUI is released under the **GNU General Public License v3.0 (GPL-3.0)**.  
This means you are free to use, modify, and redistribute PixelUI, but **any derivative work must also be licensed under GPLv3**.

👉 See the [LICENSE](./LICENSE) file for the full license text, or visit:  
[https://www.gnu.org/licenses/gpl-3.0.html](https://www.gnu.org/licenses/gpl-3.0.html)