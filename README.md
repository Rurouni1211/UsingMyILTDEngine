# ILTD-Engine

**ILTD-Engine** is a custom C++ game engine built on **DirectX 11**. Designed as a modular static library, it separates core engine logic from game implementation, allowing for rapid 2D and 3D game development.

Currently, the engine is being tested with a 2D game prototype (`EngineTestAgain`) featuring sprite rendering, transparency handling, and ImGui integration.

## 🌟 Key Features

* **Graphics Core:** DirectX 11 rendering pipeline.
* **2D Engine:**
    * Sprite rendering with transparency support.
    * Custom Sprite Batching.
    * 2D Camera & Transform controls.
* **3D Capabilities:**
    * Static & Skinned Mesh loading (`.obj`, `.fbx`).
    * Geometric primitives (Cube, Sphere, Cylinder, Capsule).
    * Basic lighting and depth management.
* **Tooling:**
    * Integrated **ImGui** editor for real-time debugging.
    * Inspector window for manipulating transforms, colors, and render states.

## 📂 Project Structure

The solution is divided into two main projects:

1.  **`ILTD-Engine` (Library):**
    * Contains the core framework, graphics wrappers, and math utilities.
    * Compiles into a `.lib` file.
2.  **`EngineTestAgain` (Game):**
    * The executable project that links against the engine.
    * Implements game-specific logic (e.g., Player movement, Level design).

## 🛠️ Getting Started

### Prerequisites
* **IDE:** Visual Studio 2022 (recommended).
* **SDKs:** Windows SDK (10.0 or newer).
* **Git:** To clone the repository.

### Building the Project

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/YourUsername/ILTD-Engine.git](https://github.com/YourUsername/ILTD-Engine.git)
    ```
2.  **Open the Solution:**
    Open `EngineTest.sln` in Visual Studio.
3.  **Build Order:**
    * Right-click `ILTD-Engine` -> **Build** (Generates the `.lib`).
    * Right-click `EngineTestAgain` -> **Set as Startup Project**.
    * Build & Run `EngineTestAgain`.

## 🚀 Usage Example

Here is how to initialize the engine and load a 2D sprite in your game's `main.cpp`:

```cpp
#include "framework.h"
#include "sprite.h"

// Global Engine Instance
framework* g_Engine = nullptr;
sprite* myPlayerSprite = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    // 1. Create Window (Standard Win32 boilerplate...)
    // ...

    // 2. Initialize Engine
    g_Engine = new framework(hwnd);

    // 3. Setup Game Assets (Callback)
    g_Engine->on_initialized = [&](framework& eng) {
        // Load a sprite from file
        myPlayerSprite = eng.create_sprite(L"Data/sprite/player.png");
        
        // Set it to active slot 0
        eng.SetSprite(0, myPlayerSprite);
        
        // Set default properties
        myPlayerSprite->movex = 100.0f;
        myPlayerSprite->movey = 100.0f;
    };

    // 4. Run Game Loop
    g_Engine->initialize();
    return g_Engine->run();
}
```

🔧 Implementation Details
Linking the Library
To use ILTD-Engine in a new project:

Go to Properties > C/C++ > General > Additional Include Directories and add the engine's header path.

Go to Properties > Linker > General > Additional Library Directories and add the output folder (e.g., $(SolutionDir)x64\Debug).

Go to Properties > Linker > Input and add ILTD-Engine.lib.

2D Rendering Pipeline
Depth Stencil: The engine automatically disables the Z-Buffer when rendering 2D sprites to ensure they draw over 3D background elements.

Blending: Alpha blending is enabled by default for sprites to support transparent textures (PNGs).

📝 Roadmap
[x] Basic Window & DirectX Setup

[x] 3D Mesh Rendering

[x] 2D Sprite Rendering

[ ] Audio System Implementation

[ ] Physics/Collision Detection (2D)

[ ] Scene Management System

Created by HEIN HTET NAUNG
