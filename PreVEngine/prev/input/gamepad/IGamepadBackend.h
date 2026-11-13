#ifndef __IGAMEPAD_BACKEND_H__
#define __IGAMEPAD_BACKEND_H__

#include "GamepadCommon.h"

#include <string>

namespace prev::input::gamepad {
struct GamepadInput {
    bool connected{ false };

    std::string name{};

    std::array<bool, GAMEPAD_BUTTON_COUNT> buttons{};

    std::array<float, GAMEPAD_AXIS_COUNT> axes{};

    bool IsPressed(const GamepadButton button) const
    {
        return buttons[static_cast<size_t>(button)];
    }

    float GetAxis(const GamepadAxis axis) const
    {
        return axes[static_cast<size_t>(axis)];
    }

    void SetButton(const GamepadButton button, const bool pressed)
    {
        buttons[static_cast<size_t>(button)] = pressed;
    }

    void SetAxis(const GamepadAxis axis, const float value)
    {
        axes[static_cast<size_t>(axis)] = value;
    }
};

using GamepadInputs = std::array<GamepadInput, MAX_GAMEPAD_COUNT>;

class IGamepadBackend {
public:
    virtual void Poll(GamepadInputs& inputs) = 0;

public:
    virtual ~IGamepadBackend() = default;
};
} // namespace prev::input::gamepad

#endif // !__IGAMEPAD_BACKEND_H__
