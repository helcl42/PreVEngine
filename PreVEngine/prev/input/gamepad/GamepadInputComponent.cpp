#include "GamepadInputComponent.h"

#include "../../event/EventChannel.h"

namespace prev::input::gamepad {
GamepadInputComponent::GamepadInputComponent()
{
    prev::event::EventChannel::Post(GamepadNamesRequest{});
}

bool GamepadInputComponent::RegisterButtonListener(IGamepadButtonListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_buttonObservers.Register(listener);
}

bool GamepadInputComponent::UnregisterButtonListener(IGamepadButtonListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_buttonObservers.Unregister(listener);
}

bool GamepadInputComponent::IsButtonListenerRegistered(IGamepadButtonListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_buttonObservers.IsRegistered(listener);
}

bool GamepadInputComponent::RegisterConnectionListener(IGamepadConnectionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_connectionObservers.Register(listener);
}

bool GamepadInputComponent::UnregisterConnectionListener(IGamepadConnectionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_connectionObservers.Unregister(listener);
}

bool GamepadInputComponent::IsConnectionListenerRegistered(IGamepadConnectionListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_connectionObservers.IsRegistered(listener);
}

std::vector<uint32_t> GamepadInputComponent::GetConnectedGamepads() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<uint32_t> result;
    for (uint32_t id = 0; id < MAX_GAMEPAD_COUNT; ++id) {
        if (m_states[id].connected) {
            result.push_back(id);
        }
    }
    return result;
}

bool GamepadInputComponent::IsConnected(const uint32_t gamepadId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return gamepadId < MAX_GAMEPAD_COUNT && m_states[gamepadId].connected;
}

bool GamepadInputComponent::IsButtonPressed(const uint32_t gamepadId, const GamepadButton button) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (gamepadId >= MAX_GAMEPAD_COUNT || button >= GamepadButton::COUNT) {
        return false;
    }
    return m_states[gamepadId].IsPressed(button);
}

float GamepadInputComponent::GetAxis(const uint32_t gamepadId, const GamepadAxis axis) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (gamepadId >= MAX_GAMEPAD_COUNT || axis >= GamepadAxis::COUNT) {
        return 0.0f;
    }
    return m_states[gamepadId].GetAxis(axis);
}

glm::vec2 GamepadInputComponent::GetStick(const uint32_t gamepadId, const GamepadStick stick) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (gamepadId >= MAX_GAMEPAD_COUNT) {
        return glm::vec2{ 0.0f };
    }
    const auto& state{ m_states[gamepadId] };
    return stick == GamepadStick::LEFT
        ? glm::vec2{ state.GetAxis(GamepadAxis::LEFT_X), state.GetAxis(GamepadAxis::LEFT_Y) }
        : glm::vec2{ state.GetAxis(GamepadAxis::RIGHT_X), state.GetAxis(GamepadAxis::RIGHT_Y) };
}

GamepadState GamepadInputComponent::GetState(const uint32_t gamepadId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (gamepadId >= MAX_GAMEPAD_COUNT) {
        return GamepadState{};
    }
    return m_states[gamepadId];
}

std::string GamepadInputComponent::GetName(const uint32_t gamepadId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (gamepadId >= MAX_GAMEPAD_COUNT) {
        return {};
    }
    return m_names[gamepadId];
}

void GamepadInputComponent::operator()(const GamepadStatesEvent& statesEvent)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_states = statesEvent.states;
}

void GamepadInputComponent::operator()(const GamepadButtonEvent& buttonEvent)
{
    std::set<IGamepadButtonListener*> observers;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        observers = m_buttonObservers.GetObservers();
    }

    for (auto* listener : observers) {
        listener->OnGamepadButton(buttonEvent);
    }
}

void GamepadInputComponent::operator()(const GamepadConnectionEvent& connectionEvent)
{
    std::set<IGamepadConnectionListener*> observers;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (connectionEvent.gamepadId < MAX_GAMEPAD_COUNT) {
            m_names[connectionEvent.gamepadId] = connectionEvent.connected ? connectionEvent.name : std::string{};
        }
        observers = m_connectionObservers.GetObservers();
    }

    for (auto* listener : observers) {
        listener->OnGamepadConnection(connectionEvent);
    }
}

void GamepadInputComponent::operator()(const GamepadNamesEvent& namesEvent)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_names = namesEvent.names;
}

} // namespace prev::input::gamepad
