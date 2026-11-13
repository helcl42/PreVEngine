#ifndef __GAME_CONTROLLER_GAMEPAD_BACKEND_H__
#define __GAME_CONTROLLER_GAMEPAD_BACKEND_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_MACOS) || defined(TARGET_PLATFORM_IOS)

#include <array>

namespace prev::input::gamepad::impl::apple {
class GameControllerGamepadBackend final : public IGamepadBackend {
public:
    GameControllerGamepadBackend() = default;

    ~GameControllerGamepadBackend() override;

    GameControllerGamepadBackend(const GameControllerGamepadBackend& other) = delete;

    GameControllerGamepadBackend& operator=(const GameControllerGamepadBackend& other) = delete;

    GameControllerGamepadBackend(GameControllerGamepadBackend&& other) = delete;

    GameControllerGamepadBackend& operator=(GameControllerGamepadBackend&& other) = delete;

public:
    void Poll(GamepadInputs& inputs) override;

private:
    std::array<void*, MAX_GAMEPAD_COUNT> m_slots{}; // retained GCController*
};
} // namespace prev::input::gamepad::impl::apple

#endif

#endif // !__GAME_CONTROLLER_GAMEPAD_BACKEND_H__
