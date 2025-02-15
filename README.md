# PreVEngine

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/88a3ed1e12e04eb09882db2c7e5ea794)](https://app.codacy.com/gh/helcl42/PreVEngine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

A multiplatform Vulkan based rendering engine. This repository is under active development.

| Platform | Status |
|---|---:|
| Windows | [![Windows - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml) |
| Linux (Wayland) | [![Linux (Wayland) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland.yml) |
| Linux (XCB) | [![Linux (XCB) - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux.yml) |
| Android  | [![Android - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml) |
| MacOS | [![MacOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-macos.yml) |
| iOS | [![iOS - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-ios.yml) |

| OpenXR Platform | Status |
|---|---:|
| Windows | [![Windows OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows%20openxr.yml) |
| Linux (Wayland) | [![Linux (Wayland) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20wayland%20openxr.yml) |
| Linux (XCB) | [![Linux (XCB) OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux%20openxr.yml) |
| Android | [![Android OpenXR - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android%20openxr.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android%20openxr.yml) |

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/lSp1hOncLVs/0.jpg)](http://www.youtube.com/watch?v=lSp1hOncLVs "PreVEngine In Action")

## Clone
- Run `git clone --recursive <url>` to init even with submodules.

## Download Dependencies
 - Install Vulkan SDK for you platform: [link](https://vulkan.lunarg.com/).
 - Run `python Scripts/download_dependencies.py` to download platform prebuilt binaries (assimp).

 **Note:** If you have a problem with the script, the package is hosted on `Google Drive`. You can download it directly from the [link](https://drive.google.com/file/d/1johmeEaVQ9SZTi13mAfJxbq17QztL-DS/view?usp=drive_link). Just unpack it in the root folder.

## Build Linux & Windows & MacOS

 - Run `mkdir build && cd build`

 - Run `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..`

 - Run `ninja`

## Build Android

 - Make sure you have `JDK-17` and `NDK-25+` installed.

 - Run `mkdir build && cd build`

 - Run `cmake -DANDROID=ON -DANDROID_ABI=arm64-v8a ..`

 - Run `cd ../PreVEngineTest/platform/android && chmod a+x gradlew && ./gradlew clean build` or import to android studio from location `PreVEngineTest/platform/android` and deploy as usuall.

 ## Build iOS

- Run `mkdir build && cd build`

- Run `cmake -G Xcode -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DCMAKE_OSX_ARCHITECTURES="arm64" -DENABLE_VK_LOADER=OFF ..`

- Open in Xcode.

- Add reference to MoltenVK framework.

- Add all assets to app bundle resources.


## 3rd Party Dependencies
 - [Assimp](https://github.com/assimp/assimp): Used for model loading.
 - [GLM](https://github.com/g-truc/glm): Great Math library.
 - [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator): Vulkan Memory Allocator.
 - [stb_image](https://github.com/nothings/stb): Used for image loading.
 - [volk](https://github.com/zeux/volk): Multiplaform meta loader for Vulkan.


