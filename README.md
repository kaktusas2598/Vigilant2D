# Vigilant2D - 2D Tiled Game Engine

Project inspired by my previous 2D engine build using SDL: [Vigilant](https://github.com/kaktusas2598/Vigilant), however I decided
to start from scratch using OpenGL for rendering and ditching SDL for GLFW.

# Included libraries

 * [GLFW](https://github.com/glfw/glfw) - A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 * [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (docking branch)
 * [stb_image](https://github.com/nothings/stb) - Public Domain Image Loading Library for C++

# Included features

 - Extended Scripting Support with Lua
   - Project content bootstrapping
   - Entity definitions and behavior scripts
   - Entity-owned and global tasks for automation
 - Tiled2D Map support
   - External tileset loading
   - Entity spawning from object layers using points
   - Collision definitions using collidable tile layer or rects in object layer
 - Simple Scene-Entity Model
 - Selectable tiles/entities
 - World and Screen space UI system
 - Particle System
 - ImgGui Debug panels
 - Integrated physics with Box2D engine
 - Animated Sprites
 - True type fonts
 - Post FX
 - Sounds

# Ubuntu instructions

Install dependencies
```
sudo apt-get install cmake make libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libglew-dev libassimp-dev libglfw3 libglfw3-dev
```
Build 
```
git submodule init && git submodule update
./configure.sh && ./build.sh
```

# TODO
  * States
  * Saving
  * Light source support
  * Pathfinding
  *
  * Real implemented game mechanics with scripting help like managing inventory, crop farming, fighting
  * ------
  * More Box2D integration
  * Hot Reload scripts, shaders, maps
  * grep -Rni "TODO" src/