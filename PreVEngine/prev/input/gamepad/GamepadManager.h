#ifndef __GAMEPAD_MANAGER_H__
#define __GAMEPAD_MANAGER_H__

#include "GamepadEvents.h"
#include "IGamepadBackend.h"

#include "../../event/EventHandler.h"

#include <array>
#include <memory>
#include <string>

namespace prev::input::gamepad {
class GamepadManager final {
public:
    GamepadManager(std::unique_ptr<IGamepadBackend> backend, const GamepadDeadZones& deadZones = {});

    ~GamepadManager() = default;

    GamepadManager(const GamepadManager& other) = delete;

    GamepadManager& operator=(const GamepadManager& other) = delete;

    GamepadManager(GamepadManager&& other) = delete;

    GamepadManager& operator=(GamepadManager&& other) = delete;

public:
    void Update();

    const GamepadStates& GetStates() const;

    const GamepadDeadZones& GetDeadZones() const;

public:
    void operator()(const GamepadDeadZonesChangeRequest& request);

    void operator()(const GamepadNamesRequest& request);

private:
    std::unique_ptr<IGamepadBackend> m_backend;

    GamepadDeadZones m_deadZones;

    GamepadStates m_states{};

    std::array<std::string, MAX_GAMEPAD_COUNT> m_names{};

    prev::event::EventHandler<GamepadManager, GamepadDeadZonesChangeRequest> m_deadZonesRequestHandler{ *this };

    prev::event::EventHandler<GamepadManager, GamepadNamesRequest> m_namesRequestHandler{ *this };
};
} // namespace prev::input::gamepad

#endif // !__GAMEPAD_MANAGER_H__
