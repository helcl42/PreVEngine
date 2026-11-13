#ifndef __IGAMEPAD_CONNECTION_LISTENER_H__
#define __IGAMEPAD_CONNECTION_LISTENER_H__

#include "GamepadEvents.h"

namespace prev::input::gamepad {
class IGamepadConnectionListener {
public:
    virtual void OnGamepadConnection(const GamepadConnectionEvent& connectionEvent) = 0;

public:
    virtual ~IGamepadConnectionListener() = default;
};
} // namespace prev::input::gamepad

#endif // !__IGAMEPAD_CONNECTION_LISTENER_H__
