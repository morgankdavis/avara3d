# Avara3D

Avara3D is a real-time 3D visualization and physical simulation engine.

Avara3D integrates real-time rendering, rigid-body physics, and fixed-step simulation into a scene-oriented C++20 API. Applications run natively on Linux, macOS, and Windows, or in modern browsers through WebAssembly and WebGL 2.

## Highlights

- A scene-oriented C++20 API
- Fixed-step simulation with pause, single-step, and bounded catch-up
- Rigid-body simulation with static, dynamic, and kinematic bodies and reusable collision shapes
- Rendering pipeline divided into scene gathering, draw packetization, and backend execution
- Separate subsystems for simulation, physics, rendering, input, and tooling
- Native Linux, macOS, and Windows targets plus WebAssembly and WebGL 2
- Runtime telemetry, CPU/GPU timing, and debug visualization

## Building

### Ubuntu

_Tested on Ubuntu 24.04.3 LTS_

#### Install build dependencies
```
sudo apt update
sudo apt install clang git cmake ninja-build libwayland-dev libx11-dev xorg-dev libxkbcommon-dev
```

#### Check out this repo

```
git clone https://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build

```
cd avara3d
cmake --preset debug \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
cmake --build --preset debug
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

#### Install CMake & Ninja

```
brew update
brew install cmake ninja
```

#### Check out this repo

```
git clone https://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build

```
cd avara3d
cmake --preset debug \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
cmake --build --preset debug
```

### Windows

_Tested on Windows 11 25H2 / build 26200.7171_

#### Enable Developer Mode

#### [Enable Developer Mode](https://learn.microsoft.com/en-us/windows/advanced-settings/developer-mode)
This is necessary to enable symlinks, which are used by A3D's CMake scripts.

#### Install [Visual Studio Community 18 2026](https://visualstudio.microsoft.com/downloads/)

* Select **Desktop development with C++**.
* On the **Individual components** tab, ensure the following components are selected:

  * Git for Windows
  * C++ Clang Compiler for Windows
  * MSBuild support for LLVM (clang-cl) toolset
  * C++ CMake tools for Windows

The CMake tools component provides the CMake and Ninja tooling used by Avara3D’s build presets.

#### Open a developer command prompt

Open Visual Studio and select **Tools → Command Line → Developer Command Prompt**. Run the remaining commands from this prompt.

#### Check out this repo

```bat
git clone https://gitlab.mkd.net/a3d/avara3d.git
```

#### Configure and build

```bat
cd avara3d
cmake --preset debug ^
    -DCMAKE_C_COMPILER=clang-cl ^
    -DCMAKE_CXX_COMPILER=clang-cl
cmake --build --preset debug
```

### Web / Emscripten

A3D executables can be compiled to WebAssembly and WebGL 2 using Emscripten.

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

From the repository root:

```sh
cmake --preset web-debug
cmake --build --preset web-debug --target <target>
```

Replace `<demo-target>` with the demo or sandbox test target you want to build.

For an optimized web build:

```sh
cmake --preset web-release
cmake --build --preset web-release --target <target>
```

Debug web builds generate an HTML launcher along with the JavaScript, WebAssembly, and asset files. Release builds produce the files needed to embed the application in a custom web page.

Web debug builds must be served over HTTP rather than opened directly with a `file://` URL. For example:

```sh
emrun cmake-build-web-debug/demos/janus/janus.html
```

### Qt / a3de

Avara3D does not depend on Qt, but the repository includes `a3de`, a minimal Qt desktop shell that may be developed further in the future. If you wish to use Avara3D in your Qt application this information may be useful.

#### Ubuntu

Install Qt 6 using the [official Qt installer](https://www.qt.io/development/download) and select a desktop GCC kit. The default installation location is `~/Qt`.

Install the additional XCB dependencies:

```sh
sudo apt install libxcb-cursor0 libxcb-cursor-dev
```

#### macOS

Install Qt 6 using the [official Qt installer](https://www.qt.io/development/download) and select the macOS desktop kit. The default installation location is `~/Qt`.

If you installed only the Xcode command-line tools and the Qt installer warns that Xcode is missing, the warning can be ignored.

#### Windows

Install Qt 6 using the [official Qt installer](https://www.qt.io/development/download).

* Select **Qt 6.x desktop development**.
* Select **Custom Installation**.
* Expand the latest Qt 6 release.
* Select **MSVC 2022 64-bit**.
* Do not select the MinGW kit.

The MSVC Qt build is compatible with Avara3D’s ClangCL build.

#### Configure and build

Configure Avara3D using the normal instructions for your platform. If CMake reports that Qt 6 was not found, rerun the configure command with the installed desktop kit directory specified as `Qt6_ROOT`:

```sh
cmake --preset debug -DQt6_ROOT="/path/to/Qt/desktop-kit"
```

Then build `a3de`:

```sh
cmake --build --preset debug --target a3de
```

Typical desktop kit directories are:

| Platform | Directory                     |
| -------- | ----------------------------- |
| Ubuntu   | `$HOME/Qt/<version>/gcc_64`   |
| macOS    | `$HOME/Qt/<version>/macos`    |
| Windows  | `C:/Qt/<version>/msvc2022_64` |

Replace `<version>` with the installed Qt version. If Qt is not found, CMake skips the `a3de` target.

