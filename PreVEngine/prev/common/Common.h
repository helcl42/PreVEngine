#ifndef __COMMON_H__
#define __COMMON_H__

#if defined(TARGET_PLATFORM_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define TICK_CHARACTER "\xFB" // On Windows, use Square-root as tick mark
#define PAUSE system("pause")
#elif defined(TARGET_PLATFORM_IOS)
#include <stdlib.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#elif TARGET_PLATFORM_MACOS
#include <stdlib.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#elif defined(TARGET_PLATFORM_ANDROID)
#include <android_native.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#elif defined(TARGET_PLATFORM_LINUX)
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#define TICK_CHARACTER "\u2713"
#define PAUSE
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <cinttypes>
#include <string>

#define STRINGIFY(f) #f

#define ALIGN_TO(alignmentInBytes) alignas(alignmentInBytes)
#define DEFAULT_ALIGNMENT ALIGN_TO(16)

#ifdef ENABLE_REVERSE_DEPTH
constexpr bool REVERSE_DEPTH{ true };
constexpr float MIN_DEPTH{ 1.0f };
constexpr float MAX_DEPTH{ 0.0f };
#else
constexpr bool REVERSE_DEPTH{ false };
constexpr float MIN_DEPTH{ 0.0f };
constexpr float MAX_DEPTH{ 1.0f };
#endif // ENABLE_REVERSE_DEPTH

constexpr uint32_t MAX_VIEW_COUNT{ 2 };

#endif
