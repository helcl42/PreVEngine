# PreVEngine

## Build in generaal

 - do not forget to set up `USE_VULKAN_WRAPPER` to `ON` or `OFF`

## Build Linux

 - Use `clang`

 - Run `export CC=/usr/bin/clang`

 - Run `export CXX=/usr/bin/clang++`

## Build Windows

- Run `mkdir build`

- Run `cd build`

- Run `cmake -G "Visual Studio 15 2017 Win64" ..`

## Build Android

 - install `NDK-20+`

 - do not forget to set up `USE_VULKAN_WRAPPER` to `ON`

 - Run `cd PreVEngineTest`

 - Run `cmake -DANDROID=ON -DABI_NAME=arm64-v8a`

 - Import to android studio



