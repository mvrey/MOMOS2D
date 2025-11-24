AncientMythos / MOMOS2D (MOMOS)
=================================

Overview
--------
This repository contains MOMOS2D — a lightweight 2D rendering utility library and a fully functional 2D game engine written in C++ that uses GLFW for rendering. It provides convenience APIs for creating a window, loading and drawing sprites, simple 2D drawing (lines, polygons), text rendering, and resource management (shaders & textures).

The engine also includes additional capabilities such as physics simulation (Chipmunk physics), local database storage (SQLite) and advanced math calculation libraries (GLM). All external dependencies can be found at src\deps.

The included demo (src/main.cc) demonstrates how to initialize a window, load sprites, draw text and primitives, and use simple transforms.

Key features
------------
- Window management and input using GLFW
- OpenGL rendering powered by GLEW
- Sprite loading via SOIL and a small Sprite API
- Shader and texture resource manager
- Simple 2D drawing helpers (lines, polygons, text)
- Font rendering using FontStash / GLFontStash
- Physics support (Chipmunk integration available)
- Local storage support using SQLite
- Advanced math via GLM

Repository layout
-----------------
- include/MOMOS/ - Public headers for the MOMOS API (window, draw, sprite, texture, resource manager, time, etc.)
- src/MOMOS/     - Implementation of the MOMOS library
- src/deps/      - Third-party dependencies (SOIL, FontStash, Chipmunk, SQLite, GLM, etc.) — all external deps used by the engine can be found here
- src/main.cc    - Small demo that shows library usage
- src/JanKenPon/assets/ - Example assets used by the demo

Dependencies
------------
The project depends on the following libraries (development headers + linked libs):
- OpenGL (desktop)
- GLEW (OpenGL extension wrangler)
- GLFW (windowing & input)
- GLM (math for matrices and advanced math)
- SOIL (image loading used for textures)
- FontStash / GLFontStash (text rendering)
- Chipmunk (2D physics)
- SQLite (local database storage)

All external third-party libraries are tracked or included under src\deps for convenience. You can inspect that folder to see exact versions and bundled sources.

On Windows, the project includes Visual Studio project files. You must ensure the include paths and library paths for the dependencies are set up in the project or installed via a package manager (vcpkg, msys2, etc.).

Build instructions (GENie / Windows / Visual Studio)
-----------------------------------------
This project uses GENie for project generation. In order to build the project download the GENie executable from https://github.com/bkaradzic/GENie and place it on your PATH or in the repository root. Then run the provided genie.bat to generate the platform project files (for example Visual Studio solution files).

Steps:
1. Download GENie (https://github.com/bkaradzic/GENie) and make genie.exe available.
2. From the repository root run: genie.bat
3. Open the generated Visual Studio solution or project and configure dependency include/lib paths if needed.
4. Build the solution.

Project output and demo
-----------------------
By default the library project is configured to compile into a static .lib library. The repository also includes a small demo (src/main.cc) so you can run the engine without external code. To run the demo directly, you can change the project type from library to executable (or create a small executable project that links the produced .lib), set main.cc as the entry point and run it. The demo will open a window and draw example sprites and text.

A precompiled static library for Windows is included in the repository at windows/debug/Momos2D.lib for convenience (Debug configuration). You can link that directly in a Visual Studio project if you need a quick start.

Quick usage (API highlights)
---------------------------
The demo (src/main.cc) shows typical usage. In short:

- Window
  - MOMOS::WindowInit(width, height)
  - while (MOMOS::WindowIsOpened()) { MOMOS::WindowFrame(); }
  - MOMOS::WindowDestroy()

- Drawing
  - MOMOS::DrawBegin(); MOMOS::DrawEnd();
  - MOMOS::DrawClear(r,g,b,a); // accepts 0-255 components
  - MOMOS::DrawLine(x1,y1,x2,y2);
  - MOMOS::DrawSetFillColor(r,g,b,a); // sets fill color & text color
  - MOMOS::DrawText(x,y, "text");

- Sprites / Textures
  - auto s = MOMOS::SpriteFromFile("path/to/image.jpg");
  - MOMOS::DrawSprite(s, x, y);
  - Sub-sprites and sprite transforms are supported (see include/MOMOS/sprite.h)

- Resource manager
  - ResourceManager::LoadShader(...) and ResourceManager::LoadTexture(...)
  - ResourceManager::GetShader("sprite") is used internally to set up sprite rendering

Assets
------
The demo references sample assets under src/JanKenPon/assets/ (flags). Ensure these files are present and the program is run with a working working directory that preserves these relative paths or update the path strings in src/main.cc.

Contributing
------------
- Fix bugs or add features via pull requests.
- Keep changes focused and provide a clear description of what you changed.

Notes and tips
--------------
- Coordinate system: the library sets up an orthographic projection where origin (0,0) is top-left; many draw functions invert the Y coordinate when calling OpenGL (see draw.cc).
- Textures: the code uses SOIL to load images and upload them to OpenGL textures (see ResourceManager and texture implementation).
- If you get blank windows or missing textures, confirm the demo can find the asset files and that the GL context initialized correctly (check output for SOIL load errors).
- The codebase includes optional subsystems (physics via Chipmunk, local storage via SQLite). If you plan to use these features make sure the corresponding headers/libs are available when generating the project with GENie.