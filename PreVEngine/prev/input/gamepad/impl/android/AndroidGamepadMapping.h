#ifndef __ANDROID_GAMEPAD_MAPPING_H__
#define __ANDROID_GAMEPAD_MAPPING_H__

#include "../../GamepadCommon.h"

#include <android/input.h>
#include <android/keycodes.h>

#include <array>
#include <cstdint>
#include <optional>

namespace prev::input::gamepad::impl::android {
inline bool IsGamepadSource(const int32_t source)
{
    return (source & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD || (source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK;
}

inline std::optional<GamepadButton> MapKey(const int32_t keyCode)
{
    switch (keyCode) {
    case AKEYCODE_BUTTON_A:
        return GamepadButton::SOUTH;
    case AKEYCODE_BUTTON_B:
        return GamepadButton::EAST;
    case AKEYCODE_BUTTON_X:
        return GamepadButton::WEST;
    case AKEYCODE_BUTTON_Y:
        return GamepadButton::NORTH;
    case AKEYCODE_BUTTON_L1:
        return GamepadButton::LEFT_SHOULDER;
    case AKEYCODE_BUTTON_R1:
        return GamepadButton::RIGHT_SHOULDER;
    case AKEYCODE_BUTTON_SELECT:
    case AKEYCODE_BACK: // some pads report their Select/View button as BACK
        return GamepadButton::BACK;
    case AKEYCODE_BUTTON_START:
        return GamepadButton::START;
    case AKEYCODE_BUTTON_MODE:
        return GamepadButton::GUIDE;
    case AKEYCODE_BUTTON_THUMBL:
        return GamepadButton::LEFT_STICK;
    case AKEYCODE_BUTTON_THUMBR:
        return GamepadButton::RIGHT_STICK;
    case AKEYCODE_DPAD_UP:
        return GamepadButton::DPAD_UP;
    case AKEYCODE_DPAD_DOWN:
        return GamepadButton::DPAD_DOWN;
    case AKEYCODE_DPAD_LEFT:
        return GamepadButton::DPAD_LEFT;
    case AKEYCODE_DPAD_RIGHT:
        return GamepadButton::DPAD_RIGHT;
    default:
        return std::nullopt;
    }
}

inline std::optional<GamepadAxis> MapDigitalTrigger(const int32_t keyCode)
{
    switch (keyCode) {
    case AKEYCODE_BUTTON_L2:
        return GamepadAxis::LEFT_TRIGGER;
    case AKEYCODE_BUTTON_R2:
        return GamepadAxis::RIGHT_TRIGGER;
    default:
        return std::nullopt;
    }
}

enum class MotionAxis : uint32_t {
    X = 0,
    Y,
    Z,
    RZ,
    RX,
    RY,
    LTRIGGER,
    RTRIGGER,
    BRAKE,
    GAS,
    HAT_X,
    HAT_Y,
    COUNT
};

constexpr size_t MOTION_AXIS_COUNT{ static_cast<size_t>(MotionAxis::COUNT) };

constexpr std::array<int32_t, MOTION_AXIS_COUNT> MOTION_AXIS_IDS{
    AMOTION_EVENT_AXIS_X,
    AMOTION_EVENT_AXIS_Y,
    AMOTION_EVENT_AXIS_Z,
    AMOTION_EVENT_AXIS_RZ,
    AMOTION_EVENT_AXIS_RX,
    AMOTION_EVENT_AXIS_RY,
    AMOTION_EVENT_AXIS_LTRIGGER,
    AMOTION_EVENT_AXIS_RTRIGGER,
    AMOTION_EVENT_AXIS_BRAKE,
    AMOTION_EVENT_AXIS_GAS,
    AMOTION_EVENT_AXIS_HAT_X,
    AMOTION_EVENT_AXIS_HAT_Y,
};

using MotionAxisValues = std::array<float, MOTION_AXIS_COUNT>;
} // namespace prev::input::gamepad::impl::android

#endif // !__ANDROID_GAMEPAD_MAPPING_H__
