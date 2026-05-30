# PreVEngine

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/88a3ed1e12e04eb09882db2c7e5ea794)](https://app.codacy.com/gh/helcl42/PreVEngine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

PreVEngine is a cross-platform rendering engine supporting both **Vulkan** and **WebGPU** (via Dawn) backends, with OpenXR support for VR/AR applications. It targets Windows, Linux (XCB/Wayland), macOS, iOS, Android, and Web (Emscripten/WebAssembly).

| Platform | Status |
|---|---:|
| Windows | [![Windows - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml) |
| Linux (Wayland) | [![Linux (Wayland) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland.yml) |
| Linux (XCB) | [![Linux (XCB) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb.yml) |
| macOS | [![MacOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml) |
| iOS | [![iOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml) |
| Android | [![Android - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml) |
| Web | [![Web - CI Build](https://github.com/helcl42/PreVEngine/actions/workflows/ci-web.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-web.yml) |

| OpenXR Platform | Status |
|---|---:|
| Windows | [![Windows OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows-openxr.yml) |
| Linux (Wayland) | [![Linux (Wayland) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-wayland-openxr.yml) |
| Linux (XCB) | [![Linux (XCB) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux-xcb-openxr.yml) |
| Android | [![Android OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android-openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android-openxr.yml) |

## 🌐 [Live Web Demo](https://helcl42.github.io/PreVEngine/)

<p align="center">
  <a href="https://helcl42.github.io/PreVEngine/">
    <img src="Examples/PreVEngineExample/assets/Textures/screenshot.png" alt="Live Web Demo Screenshot" width="720" />
  </a>
</p>

## Features

- **Multiple rendering backends**: Vulkan and WebGPU (via Dawn)
- **Cross-platform**: Win32, XCB, Wayland, Cocoa (macOS), UIKit (iOS), Android native, Web (Emscripten/WASM)
- **OpenXR integration** for VR/AR on Windows, Linux, and Android
- **Shader pipeline**: GLSL → SPIR-V → WGSL conversion

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
mkdir build && cd build
cmake -G Xcode \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=16.3 \
  -DCMAKE_OSX_ARCHITECTURES="arm64" \
  -DENABLE_VK_LOADER=OFF ..
```

Open the generated Xcode project, add MoltenVK framework reference, and add assets to app bundle resources.

### Web (Emscripten)

```bash
source /path/to/emsdk/emsdk_env.sh
mkdir build && cd build
emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Compile Shaders

```bash
# Standard (single view)
python Scripts/compile_shaders.py --spirv --max-view-count 1

# With OpenXR (stereo)
python Scripts/compile_shaders.py --spirv --max-view-count 2 --enable-xr

# Convert SPIR-V to WGSL (for WebGPU)
python Scripts/compile_shaders.py --wgsl
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_VULKAN_BACKEND` | ON | Build Vulkan backend |
| `BUILD_WEBGPU_BACKEND` | ON | Build WebGPU backend (Dawn) |
| `ENABLE_XR` | OFF | Enable OpenXR support |
| `ENABLE_LOGGING` | ON | Enable logging output |
| `ENABLE_REVERSE_DEPTH` | OFF | Use reverse depth buffer |
| `ENABLE_MULTITOUCH` | OFF | Multi-touch input support |
| `ENABLE_ASAN` | OFF | Address Sanitizer |

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
- [Assimp](https://github.com/assimp/assimp): Model loading
- [GLM](https://github.com/g-truc/glm): Math library
- [stb_image](https://github.com/nothings/stb): Image loading
- [WormHoles](https://github.com/helcl42/WormHoles): Event system

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

