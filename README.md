# PreVEngine

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/88a3ed1e12e04eb09882db2c7e5ea794)](https://app.codacy.com/gh/helcl42/PreVEngine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

PreVEngine is a cross-platform rendering engine supporting both **Vulkan** and **WebGPU** (via Dawn) backends, with OpenXR support for VR/AR applications. It targets Windows, Linux (XCB/Wayland), macOS, iOS, Android, and Web (Emscripten/WebAssembly).

| Platform | Status |
|---|---:|
| Windows | [![Windows - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml) |
| Linux (Wayland) | [![Linux (Wayland) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland.yml) |
| Linux (XCB) | [![Linux (XCB) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb.yml) |
| Linux (Headless) | [![Linux Headless - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-headless.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-headless.yml) |
| macOS | [![MacOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml) |
| iOS | [![iOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml) |
| Android | [![Android - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml) |
| Web | [![Web - CI Build](https://github.com/helcl42/PreVEngine/actions/workflows/ci-web.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-web.yml) |

| OpenXR Platform | Status |
|---|---:|
| Windows | [![Windows OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows-openxr.yml) |
| Linux (Wayland) | [![Linux (Wayland) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland-openxr.yml) |
| Linux (XCB) | [![Linux (XCB) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb-openxr.yml) |
| Linux (Headless) | [![Linux OpenXR Headless - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-openxr-headless.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-openxr-headless.yml) |
| Android | [![Android OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android-openxr.yml) |

## 🌐 [Live Web Demo](https://helcl42.github.io/PreVEngine/)

<p align="center">
  <a href="https://helcl42.github.io/PreVEngine/">
    <img src="Examples/PreVEngineExample/assets/Textures/screenshot.png" alt="Live Web Demo Screenshot" width="720" />
  </a>
</p>

<p align="center">
  Or try the minimal <a href="https://helcl42.github.io/PreVEngine/Sandbox.html"><strong>Sandbox demo</strong></a> — minimal end-to-end example (see <a href="Examples/Sandbox">Examples/Sandbox</a>).
</p>

## Features

- **Multiple rendering backends**: Vulkan and WebGPU (via Dawn)
- **Cross-platform**: Win32, XCB, Wayland, Cocoa (macOS), UIKit (iOS), Android native, Web (Emscripten/WASM)
- **OpenXR integration** for VR/AR on Windows, Linux, and Android
- **Shader pipeline**: Slang → SPIR-V / WGSL (via slangc)

## Clone

```bash
git clone https://github.com/helcl42/PreVEngine.git
cd PreVEngine
git submodule update --init
git -C PreVEngine/external/gfx submodule update --init third_party/dawn
```

## Build

### Linux / Windows / macOS

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

### Android

Requires JDK 17+ and NDK r27+.

```bash
cd Examples/PreVEngineExample/platform/android
chmod a+x gradlew && ./gradlew assembleDebug
```

With OpenXR support:

```bash
./gradlew assembleDebug -PenableXr
```

Or import `Examples/PreVEngineExample/platform/android` into Android Studio.

### iOS

```bash
mkdir build-ios && cd build-ios
cmake -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=16.3 \
  -DCMAKE_OSX_ARCHITECTURES="arm64" ..
cmake --build . --target PreVEngineExample
```

For the simulator, use `-DCMAKE_OSX_SYSROOT=iphonesimulator` instead.

### Web (Emscripten)

```bash
source /path/to/emsdk/emsdk_env.sh
mkdir build-web && cd build-web
emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target PreVEngineExample
```

### Headless (no display)

For offscreen rendering with no windowing system — e.g. a Linux server/CI box with no X11/XCB/Wayland:

```bash
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_HEADLESS=ON ..
ninja
```

`BUILD_HEADLESS` drops the windowing dependency (no X11/XCB/Wayland linked on Linux) and forces the
engine to render headless at runtime — it only ever creates an offscreen swapchain, never a surface.
For a fully windowing-free build, also pass `-DBUILD_WEBGPU_BACKEND=OFF` (Dawn pulls in X11); Vulkan
is the intended headless backend.

## Compile Shaders

Shaders are written in [Slang](https://shader-slang.com/) and compiled directly to SPIR-V and WGSL using `slangc`.

### Install Slang

Download the latest release from [github.com/shader-slang/slang/releases](https://github.com/shader-slang/slang/releases) and add `bin/` to your PATH:

```bash
# macOS (Apple Silicon)
wget https://github.com/shader-slang/slang/releases/download/v2026.9.1/slang-2026.9.1-macos-aarch64.tar.gz
tar -xzf slang-2026.9.1-macos-aarch64.tar.gz -C ~/slang --strip-components=1
export PATH="$HOME/slang/bin:$PATH"

# Linux (x86_64)
wget https://github.com/shader-slang/slang/releases/download/v2026.9.1/slang-2026.9.1-linux-x86_64-glibc-2.28.tar.gz
tar -xzf slang-2026.9.1-linux-x86_64-glibc-2.28.tar.gz -C ~/slang --strip-components=1
export PATH="$HOME/slang/bin:$PATH"

# Windows (PowerShell)
Invoke-WebRequest -Uri "https://github.com/shader-slang/slang/releases/download/v2026.9.1/slang-2026.9.1-windows-x86_64.zip" -OutFile slang.zip
Expand-Archive slang.zip -DestinationPath $env:USERPROFILE\slang
$env:PATH = "$env:USERPROFILE\slang\bin;$env:PATH"
```

### Compile

```bash
# All targets (SPIR-V + WGSL)
python Scripts/compile_shaders.py

# SPIR-V only (Vulkan)
python Scripts/compile_shaders.py --spirv

# WGSL only (WebGPU)
python Scripts/compile_shaders.py --wgsl

# With OpenXR stereo rendering
python Scripts/compile_shaders.py --enable-xr --max-view-count 2

# List all shaders
python Scripts/compile_shaders.py --list
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_VULKAN_BACKEND` | ON | Build Vulkan backend |
| `BUILD_WEBGPU_BACKEND` | ON | Build WebGPU backend (Dawn) |
| `BUILD_HEADLESS` | OFF | Build without a windowing system - offscreen only, no surface. |
| `ENABLE_XR` | OFF | Enable OpenXR support |
| `ENABLE_LOGGING` | ON | Enable logging output |
| `ENABLE_REVERSE_DEPTH` | OFF | Use reverse depth buffer |
| `ENABLE_MULTITOUCH` | OFF | Multi-touch input support |
| `ENABLE_ASAN` | OFF | Address Sanitizer (mutually exclusive with `ENABLE_TSAN`) |
| `ENABLE_UBSAN` | OFF | Undefined Behavior Sanitizer (combines with ASan or TSan) |
| `ENABLE_TSAN` | OFF | Thread Sanitizer (mutually exclusive with `ENABLE_ASAN`) |

### Linux-specific

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_XCB` | ON | XCB windowing (default on Linux) |
| `ENABLE_WAYLAND` | OFF | Wayland windowing |
| `ENABLE_XLIB` | OFF | Xlib windowing |

## Example Application

The example app supports command-line configuration:

```bash
./PreVEngineExample --width 1920 --height 1080 --fullscreen 1 --backend vulkan
```

| Option | Default | Description |
|------|---------|-------------|
| `--width` | 1600 | Window width |
| `--height` | 900 | Window height |
| `--fullscreen` | 0 | Fullscreen mode |
| `--vsync` | 1 | VSync |
| `--backend` | auto | `vulkan` or `webgpu` |
| `--sample-count` | 4 | MSAA samples |
| `--headless` | 0 | Headless rendering |

## 3rd Party Dependencies

- [gfx](https://github.com/helcl42/gfx): Graphics abstraction layer (Vulkan + WebGPU)
- [Slang](https://github.com/shader-slang/slang): Shader language and compiler
- [Assimp](https://github.com/assimp/assimp): Model loading
- [GLM](https://github.com/g-truc/glm): Math library
- [stb_image](https://github.com/nothings/stb): Image loading
- [WormHoles](https://github.com/helcl42/WormHoles): Event system

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

