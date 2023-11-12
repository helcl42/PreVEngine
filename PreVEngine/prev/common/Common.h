#ifndef __COMMON_H__
#define __COMMON_H__

#if defined(__linux__) && !defined(__ANDROID__) // Linux (desktop only)
#define __LINUX__ 1
#endif

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define TICK_CHARACTER "\xFB" // On Windows, use Square-root as tick mark
#define PAUSE system("pause")
#elif __ANDROID__
#include <android_native.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#elif __LINUX__
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <cinttypes>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#define STRINGIFY(f) #f

#define ALIGN_TO(alignmentInBytes) alignas(alignmentInBytes)
#define DEFAULT_ALIGNMENT ALIGN_TO(16)

#endif