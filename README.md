# PreVEngine

A multiplatform Vulkan based rendering engine. This repository is under active development.

Here is a video:

[![PreVEngine In Action](http://img.youtube.com/vi/lSp1hOncLVs/0.jpg)](http://www.youtube.com/watch?v=lSp1hOncLVs "PreVEngine In Action")

## Download Dependencies (glm, assimp with platform builds ~100MB)
 - Run `python download_dependencies.py`

**Note:** If you have a problem with the script, the package is hosted on `Google Drive.` You can download it directly from the following link. Just unpack it in the root folder.
[https://drive.google.com/file/d/1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c/view?usp=sharing](https://drive.google.com/file/d/1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c/view?usp=sharing)


## Build Linux & Windows

 - Run `cd build && cd build`

 - Run `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..`

 - Run `ninja`

## Build Android

 - Make sure you have `NDK-25+` installed.

 - Run `mkdir build && cd build`

 - Run `cmake -DANDROID=ON -DABI_NAME=arm64-v8a`

 - Import to android studio and deploy as usuall

## Todo:

- ...

