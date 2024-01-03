# PreVEngine

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/88a3ed1e12e04eb09882db2c7e5ea794)](https://app.codacy.com/gh/helcl42/PreVEngine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

A multiplatform Vulkan based rendering engine. This repository is under active development.

| Platform | Status |
|---|---:|
| Windows | [![Windows - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-windows.yml) |
| Linux | [![Linux - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-linux.yml) |
| Android  | [![Android - CI Build/Test](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml/badge.svg)](https://github.com/helcl42/PreVEngine/actions/workflows/ci-android.yml)  |

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/lSp1hOncLVs/0.jpg)](http://www.youtube.com/watch?v=lSp1hOncLVs "PreVEngine In Action")


## Download Dependencies (assimp with platform prebuilt binaries ~100MB)
 - Run `python Scripts/download_dependencies.py`

 **Note:** If you have a problem with the script, the package is hosted on `Google Drive`. You can download it directly from the following link. Just unpack it in the root folder.
[https://drive.google.com/file/d/1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c/view?usp=sharing](https://drive.google.com/file/d/1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c/view?usp=sharing)

## Build Linux & Windows

 - Run `mkdir build && cd build`

 - Run `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..`

 - Run `ninja`

## Build Android

 - Make sure you have `JDK-17` and `NDK-25+` installed.

 - Run `mkdir build && cd build`

 - Run `cmake -DANDROID=ON -DABI_NAME=arm64-v8a ..`

 - Run `cd ../PreVEngineTest/android && chmod a+x gradlew && ./gradlew clean build` or import to android studio and deploy as usuall from `PreVEngineTest/android`




