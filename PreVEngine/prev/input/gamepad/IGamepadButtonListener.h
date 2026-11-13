#ifndef __IGAMEPAD_BUTTON_LISTENER_H__
#define __IGAMEPAD_BUTTON_LISTENER_H__

#include "GamepadEvents.h"

namespace prev::input::gamepad {
class IGamepadButtonListener {
public:
    virtual void OnGamepadButton(const GamepadButtonEvent& buttonEvent) = 0;

public:
    virtual ~IGamepadButtonListener() = default;
};
} // namespace prev::input::gamepad

#endif // !__IGAMEPAD_BUTTON_LISTENER_H__
