#ifndef __GAMEPAD_INPUT_COMPONENT_H__
#define __GAMEPAD_INPUT_COMPONENT_H__

#include "GamepadEvents.h"
#include "IGamepadButtonListener.h"
#include "IGamepadConnectionListener.h"

#include "../../common/pattern/Observer.h"
#include "../../event/EventHandler.h"

#include <glm/vec2.hpp>

#include <array>
#include <mutex>
#include <string>
#include <vector>

namespace prev::input::gamepad {
class GamepadInputComponent final {
public:
    GamepadInputComponent();

    ~GamepadInputComponent() = default;

    GamepadInputComponent(const GamepadInputComponent& other) = delete;

    GamepadInputComponent& operator=(const GamepadInputComponent& other) = delete;

    GamepadInputComponent(GamepadInputComponent&& other) = delete;

    GamepadInputComponent& operator=(GamepadInputComponent&& other) = delete;

public:
    bool RegisterButtonListener(IGamepadButtonListener& listener);

    bool UnregisterButtonListener(IGamepadButtonListener& listener);

    bool IsButtonListenerRegistered(IGamepadButtonListener& listener) const;

    bool RegisterConnectionListener(IGamepadConnectionListener& listener);

    bool UnregisterConnectionListener(IGamepadConnectionListener& listener);

    bool IsConnectionListenerRegistered(IGamepadConnectionListener& listener) const;

public:
    std::vector<uint32_t> GetConnectedGamepads() const;

    bool IsConnected(const uint32_t gamepadId) const;

    bool IsButtonPressed(const uint32_t gamepadId, const GamepadButton button) const;

    float GetAxis(const uint32_t gamepadId, const GamepadAxis axis) const;

    glm::vec2 GetStick(const uint32_t gamepadId, const GamepadStick stick) const;

    GamepadState GetState(const uint32_t gamepadId) const;

    std::string GetName(const uint32_t gamepadId) const;

public:
    void operator()(const GamepadStatesEvent& statesEvent);

    void operator()(const GamepadButtonEvent& buttonEvent);

    void operator()(const GamepadConnectionEvent& connectionEvent);

    void operator()(const GamepadNamesEvent& namesEvent);

private:
    prev::event::EventHandler<GamepadInputComponent, GamepadStatesEvent> m_statesEventHandler{ *this };

    prev::event::EventHandler<GamepadInputComponent, GamepadButtonEvent> m_buttonEventHandler{ *this };

    prev::event::EventHandler<GamepadInputComponent, GamepadConnectionEvent> m_connectionEventHandler{ *this };

    prev::event::EventHandler<GamepadInputComponent, GamepadNamesEvent> m_namesEventHandler{ *this };

private:
    prev::common::pattern::Observer<IGamepadButtonListener> m_buttonObservers;

    prev::common::pattern::Observer<IGamepadConnectionListener> m_connectionObservers;

private:
    mutable std::mutex m_mutex;

    GamepadStates m_states{};

    std::array<std::string, MAX_GAMEPAD_COUNT> m_names{};
};
} // namespace prev::input::gamepad

#endif // !__GAMEPAD_INPUT_COMPONENT_H__
