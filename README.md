# 3D Raylib Physics Engine

[![Language: C](https://img.shields.io/badge/C-99-blue)](https://en.wikipedia.org/wiki/C99)
[![Library: raylib](https://img.shields.io/badge/raylib-5.5-green)](https://www.raylib.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**3D Physics Engine** built with [raylib](https://www.raylib.com/). Implements basic rigid body dynamics, collision detection, and FXAA post-processing for anti-aliasing.

![Demo](https://via.placeholder.com/800x400?text=Add+screenshot+here)

## Features

- ⚡ **Physics** – gravity, simple friction
- 🔍 **FXAA** – post-processing shader for anti-aliasing
- 🖱️ **Interactive** – move/throw objects with mouse/keyboard

## Installation & Build

### Requirements

- C99 compiler (GCC, Clang, MSVC)
- [CMake](https://cmake.org/) (3.16+)
- [raylib](https://github.com/raysan5/raylib) library (fetched automatically)

### Build from source

```bash
# Clone the repository
git clone https://github.com/verdeneeee/3d-raylib-physics-endgine.git
cd 3d-raylib-physics-endgine

g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o engine
./engine
