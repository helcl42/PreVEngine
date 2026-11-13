#ifndef __GAMEPAD_COMMON_H__
#define __GAMEPAD_COMMON_H__

#include <array>
#include <cstddef>
#include <cstdint>

namespace prev::input::gamepad {
constexpr uint32_t MAX_GAMEPAD_COUNT{ 4 };

enum class GamepadButton : uint32_t {
    SOUTH = 0, // Xbox A
    EAST, // Xbox B
    WEST, // Xbox X
    NORTH, // Xbox Y
    LEFT_SHOULDER,
    RIGHT_SHOULDER,
    BACK,
    START,
    GUIDE,
    LEFT_STICK,
    RIGHT_STICK,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    COUNT
};

// Sticks in [-1, 1] with +Y = up, triggers in [0, 1].
enum class GamepadAxis : uint32_t {
    LEFT_X = 0,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    LEFT_TRIGGER,
    RIGHT_TRIGGER,
    COUNT
};

enum class GamepadStick : uint32_t {
    LEFT = 0,
    RIGHT
};

constexpr size_t GAMEPAD_BUTTON_COUNT{ static_cast<size_t>(GamepadButton::COUNT) };
constexpr size_t GAMEPAD_AXIS_COUNT{ static_cast<size_t>(GamepadAxis::COUNT) };

struct GamepadState {
    bool connected{ false };

    std::array<bool, GAMEPAD_BUTTON_COUNT> buttons{};

    std::array<float, GAMEPAD_AXIS_COUNT> axes{};

    std::array<float, GAMEPAD_AXIS_COUNT> rawAxes{};

    bool IsPressed(const GamepadButton button) const
    {
        return buttons[static_cast<size_t>(button)];
    }

    float GetAxis(const GamepadAxis axis) const
    {
        return axes[static_cast<size_t>(axis)];
    }

    float GetRawAxis(const GamepadAxis axis) const
    {
        return rawAxes[static_cast<size_t>(axis)];
    }
};

using GamepadStates = std::array<GamepadState, MAX_GAMEPAD_COUNT>;

struct GamepadDeadZones {
    float stickInner{ 0.15f };

    float stickOuter{ 0.95f };

    float triggerInner{ 0.05f };

    float triggerOuter{ 0.98f };
};
} // namespace prev::input::gamepad

#endif // !__GAMEPAD_COMMON_H__
