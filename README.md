# PreVEngine

A multiplatform Vulkan based rendering engine. This repository is under active development.

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/lSp1hOncLVs/0.jpg)](http://www.youtube.com/watch?v=lSp1hOncLVs "PreVEngine In Action")

## Download Dependencies (glm, assimp with platform builds ~100MB)
 - Run `python download_dependencies.py`

 ### 
    The package is hosted on `Google Drive` and the script might not work properly. If it is your case please download zip archive manually from following link and unpack it in the root folder.
    https://drive.google.com/file/d/1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c/view?usp=sharing


## Build Linux
    In case you want to use `clang`
    - Use `clang`
    - Run `export CC=/usr/bin/clang`
    - Run `export CXX=/usr/bin/clang++`
 
 - Set up `USE_VULKAN_WRAPPER` to `OFF` in `CMakeLists.txt`
 
  - Run `mkdir build`

 - Run `cd build`

 - Run `cmake ..

 - Run `make

## Build Windows

- Set up `USE_VULKAN_WRAPPER` to `OFF` in `CMakeLists.txt`

- Run `mkdir build`

- Run `cd build`

- Run `cmake -G "Visual Studio 16 2019" -A x64 ..`

 ### 
    Runtime library dependencies are included in the downloaded pack and has to be placed in the same folder as build results.

## Build Android

 - Install `NDK-20+`

 - Set up `USE_VULKAN_WRAPPER` to `ON`in `CMakeLists.txt`

 - Run `cd PreVEngineTest`

 - Run `cmake -DANDROID=ON -DABI_NAME=arm64-v8a`

 - Import to android studio and deploy as usuall

## Todo: 

- Refactor, Refactor, ...
- Fix/Feature list is in Trello
- Integrate PreVEnginePhysycs
- Integrate PreVEngineNetwork

