# Avara3D

Avara3D is a cross-platform real-time physics and visualization engine for applied simulation.

It is designed for interactive simulations of vehicles, robots, and other physical systems, with native desktop and WebAssembly targets.

A3D’s current development focus is a browser-accessible quadrotor simulation intended to demonstrate fixed-step physics, feedback control, sensor modeling, telemetry, and real-time 3D visualization.

## Goals

* Support stable, reproducible real-time simulation workflows
* Provide rigid-body physics, visualization, telemetry, and debugging tools through a clean C++ API
* Make simulated motion and system behavior easy to understand visually
* Run on Linux, macOS, Windows, and modern web browsers
* Remain easy to integrate into focused simulation applications and technical demonstrations

## Design Principles

- Provide a simple, RealityKit-inspired public C++ API
- Keep engine internals and third-party dependencies out of the public API
- Permit ECS-style and data-oriented implementation techniques without exposing internal handles or registries
- Separate simulation, physics, rendering, input, and tooling concerns
- Use fixed-step simulation to improve stability and reproducibility
- Separate scene traversal and render-data gathering from backend drawing
- Keep application-specific code outside the engine
- Prefer straightforward, maintainable C++ over unnecessary abstraction or optimization

## What It Is Not

- A Unity/Unreal/Godot replacement. Lol.
- A generic ECS framework
- A research renderer

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
cd avara3d
mkdir build
cd build
cmake .. -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_PREFIX_PATH="$HOME/Qt/6.10.1/gcc_64" -GNinja
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
cmake -DCMAKE_PREFIX_PATH="$HOME/Qt/6.10.1/macos" -GNinja ..
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

### Web / Emscripten

A3D demos can be compiled to WebAssembly and WebGL 2 using Emscripten.

#### Install Emscripten

Install and activate the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

On Linux or macOS, activate the SDK in the current shell:

```sh
source /path/to/emsdk/emsdk_env.sh
```

On Windows, activate it from Command Prompt:

```bat
C:\path\to\emsdk\emsdk_env.bat
```

#### Configure and build

Run these commands from the repository root:

```sh
cmake --preset web-debug
cmake --build cmake-build-web-debug --target <demo-target>
```

Replace `<demo-target>` with the demo or test target you want to build.

For an optimized web build:

```sh
cmake --preset web-release
cmake --build cmake-build-web-release --target <demo-target>
```

Debug web builds generate an HTML launcher alongside the JavaScript, WebAssembly, and asset files. Release builds produce the files needed to embed the application in a custom web page.

Web debug builds must be served over HTTP rather than opened directly with a `file://` URL. For example:

```sh
emrun cmake-build-web-debug/tests/000-sandbox/000-sandbox.html
```

### Automated tests

A3D's automated tests use Catch2 3. Catch2 is optional and is only required to build the automated test suite. If Catch2 is not found, CMake prints a warning and skips the automated tests.

Both automated and sandbox tests are enabled by default.

#### Ubuntu

```sh
sudo apt install catch2
```

#### macOS

```sh
brew install catch2
```

#### Windows

Install Catch2 using vcpkg or another CMake-compatible package installation.

Automated and sandbox tests can be enabled or disabled independently:

```sh
-DA3D_BUILD_AUTOMATED_TESTS=ON
-DA3D_BUILD_SANDBOX_TESTS=ON
```

Run the automated tests with:

```sh
ctest --test-dir cmake-build-debug --output-on-failure
```
