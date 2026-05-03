# Vigilant2D - 2D Tiled Game Engine

Project inspired by my previous 2D engine build using SDL: [Vigilant](https://github.com/kaktusas2598/Vigilant), however I decided
to start from scratch using OpenGL for rendering and ditching SDL for GLFW.

# Included libraries

 * [GLFW](https://github.com/glfw/glfw) - A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 * [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (docking branch)
 * [stb_image](https://github.com/nothings/stb) - Public Domain Image Loading Library for C++

# Included features

 * Simple Scene-Entity Model
 * Particle System
 * ImgGui Debug tools
 * Tiled Map (.TMX) loading, selectable tile cursor

# Ubuntu instructions

```
sudo apt-get install cmake make libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libglew-dev libassimp-dev libglfw3 libglfw3-dev
```

# TODO in order of importance
  * Entity system (something simple, not ECS)
  * Selectable entities/tiles
  * UI System (world and screen space)
  * Scripting
  * Saving
  * Box2D integration
  * Light source support
  * True type fonts
  * Sound
  *
  * ------
  * Cutscene, cinematic camera support (Lua Coroutines?)
  * Hot Reload scripts, shaders, maps

