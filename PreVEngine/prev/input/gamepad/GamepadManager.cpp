#include "GamepadManager.h"

#include "GamepadUtils.h"

#include "../../common/Logger.h"
#include "../../event/EventChannel.h"

namespace prev::input::gamepad {
namespace {
    void PostButtonEvent(const uint32_t gamepadId, const size_t buttonIndex, const bool pressed)
    {
        prev::event::EventChannel::Post(GamepadButtonEvent{ gamepadId, pressed ? GamepadButtonActionType::PRESS : GamepadButtonActionType::RELEASE, static_cast<GamepadButton>(buttonIndex) });
    }

    GamepadState ToState(const GamepadInput& input, const GamepadDeadZones& deadZones)
    {
        GamepadState state{};
        if (!input.connected) {
            return state;
        }
        state.connected = true;
        state.buttons = input.buttons;
        state.rawAxes = input.axes;

        const auto filterStick = [&](const GamepadAxis xAxis, const GamepadAxis yAxis) {
            const glm::vec2 stick{ util::ApplyRadialDeadZone({ input.axes[static_cast<size_t>(xAxis)], input.axes[static_cast<size_t>(yAxis)] }, deadZones.stickInner, deadZones.stickOuter) };
            state.axes[static_cast<size_t>(xAxis)] = stick.x;
            state.axes[static_cast<size_t>(yAxis)] = stick.y;
        };
        filterStick(GamepadAxis::LEFT_X, GamepadAxis::LEFT_Y);
        filterStick(GamepadAxis::RIGHT_X, GamepadAxis::RIGHT_Y);
        for (const auto trigger : { GamepadAxis::LEFT_TRIGGER, GamepadAxis::RIGHT_TRIGGER }) {
            state.axes[static_cast<size_t>(trigger)] = util::ApplyTriggerDeadZone(input.axes[static_cast<size_t>(trigger)], deadZones.triggerInner, deadZones.triggerOuter);
        }
        return state;
    }
} // namespace

GamepadManager::GamepadManager(std::unique_ptr<IGamepadBackend> backend, const GamepadDeadZones& deadZones)
    : m_backend{ std::move(backend) }
    , m_deadZones{ deadZones }
{
}

void GamepadManager::Update()
{
    if (!m_backend) {
        return;
    }

    GamepadInputs inputs{};
    m_backend->Poll(inputs);

    GamepadStates current{};
    for (uint32_t id = 0; id < MAX_GAMEPAD_COUNT; ++id) {
        current[id] = ToState(inputs[id], m_deadZones);
    }

    const GamepadStates previous{ m_states };
    m_states = current;

    prev::event::EventChannel::Post(GamepadStatesEvent{ m_states });

    for (uint32_t id = 0; id < MAX_GAMEPAD_COUNT; ++id) {
        const auto& before{ previous[id] };
        const auto& after{ current[id] };

        if (before.connected && !after.connected) {
            for (size_t button = 0; button < GAMEPAD_BUTTON_COUNT; ++button) {
                if (before.buttons[button]) {
                    PostButtonEvent(id, button, false);
                }
            }
            LOGI("Gamepad %u disconnected: %s", id, m_names[id].c_str());
            prev::event::EventChannel::Post(GamepadConnectionEvent{ id, false, m_names[id] });
            m_names[id].clear();
            continue;
        }

        if (!before.connected && after.connected) {
            m_names[id] = inputs[id].name;
            LOGI("Gamepad %u connected: %s", id, m_names[id].c_str());
            prev::event::EventChannel::Post(GamepadConnectionEvent{ id, true, m_names[id] });
        }

        for (size_t button = 0; button < GAMEPAD_BUTTON_COUNT; ++button) {
            if (before.buttons[button] != after.buttons[button]) {
                PostButtonEvent(id, button, after.buttons[button]);
            }
        }
    }
}

const GamepadStates& GamepadManager::GetStates() const
{
    return m_states;
}

const GamepadDeadZones& GamepadManager::GetDeadZones() const
{
    return m_deadZones;
}

void GamepadManager::operator()(const GamepadDeadZonesChangeRequest& request)
{
    m_deadZones = request.deadZones;
}

void GamepadManager::operator()(const GamepadNamesRequest& request)
{
    prev::event::EventChannel::Post(GamepadNamesEvent{ m_names });
}
} // namespace prev::input::gamepad
