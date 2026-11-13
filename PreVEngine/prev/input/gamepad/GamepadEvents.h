#ifndef __GAMEPAD_EVENTS_H__
#define __GAMEPAD_EVENTS_H__

#include "GamepadCommon.h"

#include <string>

namespace prev::input::gamepad {
enum class GamepadButtonActionType : uint32_t {
    PRESS = 0,
    RELEASE = 1
};

struct GamepadStatesEvent {
    GamepadStates states{};
};

struct GamepadConnectionEvent {
    uint32_t gamepadId{};

    bool connected{};

    std::string name{};
};

struct GamepadButtonEvent {
    uint32_t gamepadId{};

    GamepadButtonActionType action{};

    GamepadButton button{};
};

struct GamepadDeadZonesChangeRequest {
    GamepadDeadZones deadZones{};
};

struct GamepadNamesRequest {
};

struct GamepadNamesEvent {
    std::array<std::string, MAX_GAMEPAD_COUNT> names{};
};
} // namespace prev::input::gamepad

#endif // !__GAMEPAD_EVENTS_H__
