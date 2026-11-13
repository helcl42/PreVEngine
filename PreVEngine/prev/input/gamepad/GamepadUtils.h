#ifndef __GAMEPAD_UTILS_H__
#define __GAMEPAD_UTILS_H__

#include "GamepadCommon.h"

#include <glm/vec2.hpp>

#include <algorithm>
#include <cmath>

namespace prev::input::gamepad::util {
inline float Rescale(const float value, const float inner, const float outer)
{
    if (value <= inner) {
        return 0.0f;
    }
    if (outer <= inner) {
        return 1.0f;
    }
    return std::min((value - inner) / (outer - inner), 1.0f);
}

inline glm::vec2 ApplyRadialDeadZone(const glm::vec2& stick, const float inner, const float outer)
{
    const float magnitude{ std::sqrt(stick.x * stick.x + stick.y * stick.y) };
    if (magnitude <= inner || magnitude <= 0.0f) {
        return glm::vec2{ 0.0f };
    }
    return stick * (Rescale(magnitude, inner, outer) / magnitude);
}

inline float ApplyTriggerDeadZone(const float trigger, const float inner, const float outer)
{
    return Rescale(std::clamp(trigger, 0.0f, 1.0f), inner, outer);
}

inline float NormalizeCentered(const int32_t value, const int32_t min, const int32_t max)
{
    if (max <= min) {
        return 0.0f;
    }
    const float center{ (static_cast<float>(min) + static_cast<float>(max)) * 0.5f };
    const float halfRange{ (static_cast<float>(max) - static_cast<float>(min)) * 0.5f };
    return std::clamp((static_cast<float>(value) - center) / halfRange, -1.0f, 1.0f);
}

inline float NormalizeUnsigned(const int32_t value, const int32_t min, const int32_t max)
{
    if (max <= min) {
        return 0.0f;
    }
    return std::clamp((static_cast<float>(value) - static_cast<float>(min)) / (static_cast<float>(max) - static_cast<float>(min)), 0.0f, 1.0f);
}
} // namespace prev::input::gamepad::util

#endif // !__GAMEPAD_UTILS_H__
