#include "WebGamepadBackend.h"

#if defined(TARGET_PLATFORM_EMSCRIPTEN)

#include "../../GamepadUtils.h"

#include "../../../../common/Logger.h"

#include <emscripten/html5.h>

#include <algorithm>
#include <cstring>
#include <iterator>
#include <optional>

namespace prev::input::gamepad::impl::emscripten {
namespace {
    constexpr std::optional<GamepadButton> STANDARD_BUTTONS[] = {
        GamepadButton::SOUTH,
        GamepadButton::EAST,
        GamepadButton::WEST,
        GamepadButton::NORTH,
        GamepadButton::LEFT_SHOULDER,
        GamepadButton::RIGHT_SHOULDER,
        std::nullopt, // left trigger, read as an axis
        std::nullopt, // right trigger, read as an axis
        GamepadButton::BACK,
        GamepadButton::START,
        GamepadButton::LEFT_STICK,
        GamepadButton::RIGHT_STICK,
        GamepadButton::DPAD_UP,
        GamepadButton::DPAD_DOWN,
        GamepadButton::DPAD_LEFT,
        GamepadButton::DPAD_RIGHT,
        GamepadButton::GUIDE,
    };
} // namespace

void WebGamepadBackend::Poll(GamepadInputs& inputs)
{
    if (emscripten_sample_gamepad_data() != EMSCRIPTEN_RESULT_SUCCESS) {
        return;
    }

    const int count{ emscripten_get_num_gamepads() };
    for (auto& index : m_slotIndices) {
        EmscriptenGamepadEvent probe{};
        if (index >= 0 && (index >= count || emscripten_get_gamepad_status(index, &probe) != EMSCRIPTEN_RESULT_SUCCESS || !probe.connected)) {
            index = -1;
        }
    }
    for (int index = 0; index < count; ++index) {
        EmscriptenGamepadEvent probe{};
        if (std::find(m_slotIndices.begin(), m_slotIndices.end(), index) != m_slotIndices.end()
            || emscripten_get_gamepad_status(index, &probe) != EMSCRIPTEN_RESULT_SUCCESS || !probe.connected) {
            continue;
        }
        const auto freeSlot{ std::find(m_slotIndices.begin(), m_slotIndices.end(), -1) };
        if (freeSlot == m_slotIndices.end()) {
            break;
        }
        *freeSlot = index;
    }

    for (uint32_t slot = 0; slot < MAX_GAMEPAD_COUNT; ++slot) {
        EmscriptenGamepadEvent pad{};
        if (m_slotIndices[slot] < 0 || emscripten_get_gamepad_status(m_slotIndices[slot], &pad) != EMSCRIPTEN_RESULT_SUCCESS || !pad.connected) {
            continue;
        }
        if (std::strcmp(pad.mapping, "standard") != 0 && !m_warnedNonStandard) {
            m_warnedNonStandard = true;
            LOGW("Gamepad: '%s' has no standard browser mapping - buttons may be mislabeled", pad.id);
        }

        GamepadInput& input{ inputs[slot] };
        input.connected = true;
        input.name = pad.id;

        for (int button = 0; button < pad.numButtons && button < static_cast<int>(std::size(STANDARD_BUTTONS)); ++button) {
            if (STANDARD_BUTTONS[button]) {
                input.SetButton(*STANDARD_BUTTONS[button], pad.digitalButton[button] != 0);
            }
        }
        if (pad.numButtons > 7) {
            input.SetAxis(GamepadAxis::LEFT_TRIGGER, static_cast<float>(pad.analogButton[6]));
            input.SetAxis(GamepadAxis::RIGHT_TRIGGER, static_cast<float>(pad.analogButton[7]));
        }
        if (pad.numAxes > 3) {
            input.SetAxis(GamepadAxis::LEFT_X, static_cast<float>(pad.axis[0]));
            input.SetAxis(GamepadAxis::LEFT_Y, -static_cast<float>(pad.axis[1]));
            input.SetAxis(GamepadAxis::RIGHT_X, static_cast<float>(pad.axis[2]));
            input.SetAxis(GamepadAxis::RIGHT_Y, -static_cast<float>(pad.axis[3]));
        }
    }
}
} // namespace prev::input::gamepad::impl::emscripten

#endif
