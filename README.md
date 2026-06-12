# Avara3D

Avara3D is a small RealityKit-ish game and rendering engine written in C++ using OpenGL. The name comes from Avara, the classic Mac game by Ambrosia Software.

## Goals

- Create a simple, easy to use library for games and 3D applications
- Incorporate modern engine architecture techniques
- Run on modern Linux, macOS, Windows, and web browsers
- Create some interesting demos, and eventually a simple Avara-style multiplayer game using the library
- Learn something

## Core Design Principles

- RealityKit-ish object model: World, Entity, components, resources
- ECS-ish internals with a clear API boundary
- Use modern architecture patterns:
    - Resource handles
    - Extracted rendering
    - Fixed-step simulation
    - Explicit frame lifetime
- Keep rendering, physics, audio, scripting, and tools separated
- Keep engine internals and third-party dependencies out of the public API
- Keep application code out of the engine
- When in doubt, choose simple over fast, within reason

## Roadmap

- Milestone 1: Core Architecture & Playable Demo
- Milestone 2: Buildout
- Milestone 3: Multiplayer MVP
- Milestone 4: Vulkan Backend

See [ROADMAP.txt](https://gitlab.mkd.net/a3d/avara3d/-/snippets/90/raw/master/ROADMAP.txt)

## What It Is Not

- A Unity/Unreal/Godot replacement. Lol.
- A generic ECS framework
- A research renderer
- Perfect

## Building

### Ubuntu

_Tested on Ubuntu 24.04.3 LTS_

#### Install build dependencies
```
sudo apt update
sudo apt install git cmake ninja-build libwayland-dev libx11-dev xorg-dev libxkbcommon-dev
```

#### Optional: Install Qt and additional dependencies to build the a3de editor
[Use the official installer](https://www.qt.io/development/download) ("Qt Framework and Tools")
Take note of the install location (defaults to ~/Qt).

```
sudo apt install libxcb-cursor0 libxcb-cursor-dev
```

#### Check out this repo

```
git clone http://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build
If building with Qt/a3de, substitute your real Qt install location & version in CMAKE_PREFIX_PATH

```
mkdir build
cd build
cmake .. -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_PREFIX_PATH="~/Qt/6.10.1/gcc_64" -GNinja
cmake --build . -j
```


### macOS

_Tested on macOS Sequoia 15.7.2_

#### Install Xcode or just the Xcode CLI tools

```
xcode-select --install
```

#### Install Homebrew

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### Install cmake & ninja

```
brew update
brew install cmake ninja
```

#### Optional: Install Qt to build the a3de editor
Use [the official installer](https://www.qt.io/development/download) ("Qt Framework and Tools")
Take note of the install location (defaults to ~/Qt).
If you only installed the Xcode CLI tools and the Qt installer warns about missing Xcode, ignore it.
    
#### Check out this repo

```
git clone http://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build
If building with Qt/a3de, substitute your real Qt install location in CMAKE_PREFIX_PATH

```
cd avara3d
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="~/Qt/6.10.1/macos" -GNinja ..
cmake --build . -j$(sysctl -n hw.ncpu)
```

### Windows

_Tested on Windows 11 25H2 / build 26200.7171_

#### [Enable Developer Mode](https://learn.microsoft.com/en-us/windows/advanced-settings/developer-mode)
This is necessary to enable symlinks, which are used by A3D's CMake scripts.

#### Install [Visual Studio Community 18 2026](https://visualstudio.microsoft.com/downloads/)
- Check "Desktop development with C++"
- Click the "Individual components" tab, add the following components:
    - Git for Windows
    - C++ Clang Compiler for Windows (20.1.8)
    - MSBuild support for LLVM (clang-cl) toolset

#### Install [CMake](https://cmake.org/download/)

#### Optional: Install Qt to build the a3de editor
Use [the official installer](https://www.qt.io/development/download) ("Qt Framework and Tools")
- Check "Qt 6.x desktop development"
- Check "Custom Installation"
- Hit "Next"
    - Expand "Qt 6.x" (latest)
    - Uncheck "MinGW x.y.z 64-bit"
    - Check "MSVC 2022 64-bit"

#### Check out this repo

```
git clone http://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build

```
cd avara3d
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/msvc2022_64" -TClangCL ..
cmake --build . -j
```
