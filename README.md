# GLFW
OpenGL/GLM/ImGui/GLFW project template

# Included libraries

 * [GLFW](https://github.com/glfw/glfw) - A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 * [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (docking branch)
 * [stb_image](https://github.com/nothings/stb) - Public Domain Image Loading Library for C++

# Included features

 * OpenGL Abstractions:
   - Texture with texture loading using stb_image
   - Shader
   - FrameBuffer
   - GL Error handling macro
 * Helpers:
   - Utility method to load file onto a buffer
   - ImGui abstraction class

# Ubuntu instructions

```
sudo apt-get install cmake make libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libglew-dev libassimp-dev libglfw3 libglfw3-dev
```

![DEMO](screenshot.png?raw=true "Minimal skeleton demo application")
