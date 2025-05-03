# PreVEngine

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/88a3ed1e12e04eb09882db2c7e5ea794)](https://app.codacy.com/gh/helcl42/PreVEngine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

PreVEngine is a Vulkan-based rendering engine with OpenXR support, designed for cross-platform development. It provides a modular architecture for rendering, input handling, and XR integration.

| Platform | Status |
|---|---:|
| Windows | [![Windows - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml) |
| Linux (Wayland) | [![Linux (Wayland) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland.yml) |
| Linux (XCB) | [![Linux (XCB) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20xcb.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20xcb.yml) |
| Android  | [![Android - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml) |
| MacOS | [![MacOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml) |
| iOS | [![iOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml) |

| OpenXR Platform | Status |
|---|---:|
| Windows | [![Windows OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows%20openxr.yml) |
| Linux (Wayland) | [![Linux (Wayland) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland%20openxr.yml) |
| Linux (XCB) | [![Linux (XCB) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20xcb%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20xcb%20openxr.yml) |
| Android | [![Android OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android%20openxr.yml) |

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/lSp1hOncLVs/0.jpg)](http://www.youtube.com/watch?v=lSp1hOncLVs "PreVEngine In Action")

## Clone
```bash
git clone --recurse-submodules <url>` # to clone with submodules.
```

## Download Dependencies
 - Install Vulkan SDK for you platform: [link](https://vulkan.lunarg.com/).
 - Download platform prebuilt binaries (assimp) by executing the follwing command.
 ```bash
 python Scripts/download_dependencies.py
 ```

 **Note:** If you have a problem with the script, the package is hosted on `Google Drive`. You can download it directly from the [link](https://drive.google.com/file/d/1johmeEaVQ9SZTi13mAfJxbq17QztL-DS/view?usp=drive_link). Just unpack it in the root folder of the repositor.

## Build Linux & Windows & MacOS
```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Build Android
- Make sure you have `JDK-17` and `NDK-25+` installed.
```bash
mkdir build && cd build
cmake -DANDROID=ON -DANDROID_ABI=arm64-v8a ..
cd ../PreVEngineTest/platform/android
chmod a+x gradlew && ./gradlew clean build
```
*Alternatively import to android studio from location `PreVEngineTest/platform/android` and deploy as usuall*

 ## Build iOS
- Generate Xcode project 
```bash
mkdir build && cd build
cmake -G Xcode -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DCMAKE_OSX_ARCHITECTURES="arm64" -DENABLE_VK_LOADER=OFF ..
```
- Open in Xcode.
- Add reference to MoltenVK framework.
- Add all assets to app bundle resources.

## OpenXR
- To enable OpenXR enable cmake option
```cmake
option(ENABLE_XR "Enable XR" ON)
```

## Compile Shaders
The project provides a script for batch shader compilation
```bash
# without OpenXR
python Scripts/compile_shaders.py --input_folder PreVEngineTest/assets/Shaders/ --output_folder build/PreVEngineTest/assets/Shaders/ --compile_serial --compiler_args '-DMAX_VIEW_COUNT=1 ' --force_compile_all

# with OpenXR enabled 
python Scripts/compile_shaders.py --input_folder PreVEngineTest/assets/Shaders/ --output_folder build/PreVEngineTest/assets/Shaders/ --compile_serial --compiler_args '-DMAX_VIEW_COUNT=2 ' --compiler_args '-DENABLE_XR=1 ' --force_compile_all

```

## 3rd Party Dependencies
 - [Assimp](https://github.com/assimp/assimp): Used for model loading.
 - [GLM](https://github.com/g-truc/glm): Great Math library.
 - [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator): Vulkan Memory Allocator.
 - [stb_image](https://github.com/nothings/stb): Used for image loading.
 - [volk](https://github.com/zeux/volk): Multiplaform meta loader for Vulkan.


