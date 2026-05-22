# Vigilant2D - 2D Tiled Game Engine

Project inspired by my previous 2D engine build using SDL: [Vigilant](https://github.com/kaktusas2598/Vigilant), however I decided
to start from scratch using OpenGL for rendering and ditching SDL for GLFW.

![Farming Game Demo](assets/Screenshot%202026-05-16.png)

# Used libraries

 * [GLFW](https://github.com/glfw/glfw) - A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 * [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (docking branch)
 * [stb_image](https://github.com/nothings/stb) - Public Domain Image Loading Library for C++
 * [TinyXML-2](https://github.com/leethomason/tinyxml2) - Simple, small, efficient C++ XML parser
 * [FreeType](https://freetype.org/) - Freely available software library to render fonts
 * [OpenAL](https://www.openal.org/) - Cross platform 3D Audio API
 * [libsndfile](https://github.com/libsndfile/libsndfile) - A C library for reading and writing sound files
 * [Box2D](https://box2d.org/) - A 2D physics engine for games
 * [Lua](https://www.lua.org/) - For all your scripting needs

# Included features

 - Extended Scripting Support with Lua
   - Project content bootstrapping
   - Entity definitions with generic data definitions accessed by engine and scripts
   - Entity-driven behavior scripts
   - Entity-owned and global tasks for automation/cutscenes
   - Screen flow scripting: base and overlay screens can be set
   - Bootstrap script to specify map, parameters, etc.
 - Tiled2D Map support
   - External tileset loading
   - Entity spawning from object layers using points
   - Collision definitions using collidable tile layer or rects in object layer
   - Animated tiles
   - Background/foreground layers relative to entities
 - Retained Mode UI System
   - Button, Label, Image, Progress Bar, Slot Strip Widgets
   - Layout making using containers and pivots/anchors
   - Renderable in world and screen space
   - Fully scriptable from Lua 
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
 - Custom 2D Data Grid containers to define any game specific data

# Ubuntu instructions

Install dependencies
```
sudo apt-get install cmake make libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libglew-dev libglfw3 libglfw3-dev libfreetype6-dev libopenal1 libsndfile1 pkg-config
```
Build
```
git submodule init && git submodule update
./configure.sh && ./build.sh
```

# TODO
  * Light source support
  * Pathfinding
  * More POST FX/ Blur, etc.
  * Real implemented game mechanics with scripting help like managing inventory, crop farming, random encounters, etc, do research here
  * Game Events/ Triggers
  * Some kind of backgrounds, fog of war
  * Warping to other maps, rooms, caves, etc.
  * Scriptable dialogue?
  * Saving/serialisation
  * More Box2D integration
  * Hot Reload scripts, shaders, maps
  * Layered sprites
  * grep -Rni "TODO" src/
