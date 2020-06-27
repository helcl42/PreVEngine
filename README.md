# PreVEngine

A multiplatform Vulkan based rendering engine. This repository is under active development.

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/S9Hq21d241k/0.jpg)](http://www.youtube.com/watch?v=S9Hq21d241k "PreVEngine In Action")

Todo: 
- Refactor, Refactor, ...
- Integrate PreVEnginePhysycs
- Integrate PreVEngineNetwork
- Featurelist is in Trello

## Build Linux

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

- Run `cmake -G "Visual Studio 15 2017 Win64" ..`

## Build Android

 - Install `NDK-20+`

 - Set up `USE_VULKAN_WRAPPER` to `ON`in `CMakeLists.txt`

 - Run `cd PreVEngineTest`

 - Run `cmake -DANDROID=ON -DABI_NAME=arm64-v8a`

 - Import to android studio and deploy as usuall



