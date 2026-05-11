# AGENTS.md

## Project

Custom 2D OpenGL game engine written in C++ using Lua for Content Bootstrapping and scripting

Development environments:
- Linux with Neovim/tmux
- Windows with VS Code and MSYS2 MinGW64 shell

Build system:
- CMake
- `configure.sh`
- `build.sh`

Goals:
- Build a reusable 2D tiled game engine.
- Implement at least one game that demonstrates the engine, this being a farming type of 2D top down 
- Almost any 2D tiled game can be bootstrapped and implemented using Lua and some kind of engine lib/editor
- Refer to `README.md` for current goals, TODOs, and planned features.

## Coding Philosophy

Prefer:
- Always follow these principles: ETC(Easy to change), DRY and Ortogonal
- Composition over inheritance
- Explicit ownership
- Incremental changes
- Readable code
- Minimal dependencies

Avoid:
- Giant rewrites
- Premature optimization
- Hidden magic
- Overengineered ECS patterns

## Rendering

Current renderer:
- OpenGL 4.x
- Orthographic camera
- Sprite batching

## Entity/Game Architecture

Current approach:
- Simple entity/component structures
- No full ECS framework

## Current Priorities

For current priorities, refer to `README.md` or the project TODO list.

Do not assume the active task from this file alone.

## How To Help

When making suggestions:
- Explain reasoning
- Prefer small modifications
- Avoid rewriting working systems
- Check existing project style before introducing new patterns

Before suggesting major changes:
- Ask whether simplicity or scalability is preferred.