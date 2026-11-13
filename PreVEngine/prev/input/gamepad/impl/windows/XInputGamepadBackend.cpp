#include "XInputGamepadBackend.h"

#if defined(TARGET_PLATFORM_WINDOWS)

#include "../../GamepadUtils.h"

#include "../../../../common/Common.h"
#include "../../../../common/Logger.h"

#include <xinput.h>

namespace prev::input::gamepad::impl::windows {
namespace {
    using XInputGetStateFunc = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);

    // Probing an empty user index is slow.
    constexpr auto PROBE_INTERVAL{ std::chrono::milliseconds(1000) };

    struct ButtonMapping {
        WORD mask;

        GamepadButton button;
    };

    constexpr ButtonMapping BUTTON_MAPPINGS[] = {
        { XINPUT_GAMEPAD_A, GamepadButton::SOUTH },
        { XINPUT_GAMEPAD_B, GamepadButton::EAST },
        { XINPUT_GAMEPAD_X, GamepadButton::WEST },
        { XINPUT_GAMEPAD_Y, GamepadButton::NORTH },
        { XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton::LEFT_SHOULDER },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton::RIGHT_SHOULDER },
        { XINPUT_GAMEPAD_BACK, GamepadButton::BACK },
        { XINPUT_GAMEPAD_START, GamepadButton::START },
        { XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton::LEFT_STICK },
        { XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton::RIGHT_STICK },
        { XINPUT_GAMEPAD_DPAD_UP, GamepadButton::DPAD_UP },
        { XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton::DPAD_DOWN },
        { XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton::DPAD_LEFT },
        { XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton::DPAD_RIGHT },
    };

    float NormalizeThumb(const SHORT value)
    {
        return util::NormalizeCentered(value, -32768, 32767);
    }
} // namespace

XInputGamepadBackend::XInputGamepadBackend()
{
    for (const char* dll : { "xinput1_4.dll", "xinput9_1_0.dll", "xinput1_3.dll" }) {
        if (HMODULE module = LoadLibraryA(dll)) {
            m_module = module;
            m_getState = reinterpret_cast<void*>(GetProcAddress(module, "XInputGetState"));
            if (m_getState) {
                LOGI("Gamepad: using %s", dll);
                break;
            }
            FreeLibrary(module);
            m_module = nullptr;
        }
    }
    if (!m_getState) {
        LOGW("Gamepad: XInput not available - gamepads disabled");
    }
}

XInputGamepadBackend::~XInputGamepadBackend()
{
    if (m_module) {
        FreeLibrary(static_cast<HMODULE>(m_module));
    }
}

void XInputGamepadBackend::Poll(GamepadInputs& inputs)
{
    if (!m_getState) {
        return;
    }
    const auto getState{ reinterpret_cast<XInputGetStateFunc>(m_getState) };
    const auto now{ Clock::now() };

    for (DWORD index = 0; index < MAX_GAMEPAD_COUNT; ++index) {
        auto& input{ inputs[index] };
        if (!m_connected[index] && now < m_nextProbe[index]) {
            input = GamepadInput{};
            continue;
        }

        XINPUT_STATE xinputState{};
        m_connected[index] = getState(index, &xinputState) == ERROR_SUCCESS;
        if (!m_connected[index]) {
            m_nextProbe[index] = now + PROBE_INTERVAL;
            input = GamepadInput{};
            continue;
        }

        const XINPUT_GAMEPAD& pad{ xinputState.Gamepad };
        input.connected = true;
        input.name = "XInput controller " + std::to_string(index + 1);
        for (const auto& mapping : BUTTON_MAPPINGS) {
            input.SetButton(mapping.button, (pad.wButtons & mapping.mask) != 0);
        }
        input.SetAxis(GamepadAxis::LEFT_X, NormalizeThumb(pad.sThumbLX));
        input.SetAxis(GamepadAxis::LEFT_Y, NormalizeThumb(pad.sThumbLY));
        input.SetAxis(GamepadAxis::RIGHT_X, NormalizeThumb(pad.sThumbRX));
        input.SetAxis(GamepadAxis::RIGHT_Y, NormalizeThumb(pad.sThumbRY));
        input.SetAxis(GamepadAxis::LEFT_TRIGGER, util::NormalizeUnsigned(pad.bLeftTrigger, 0, 255));
        input.SetAxis(GamepadAxis::RIGHT_TRIGGER, util::NormalizeUnsigned(pad.bRightTrigger, 0, 255));
    }
}
} // namespace prev::input::gamepad::impl::windows

#endif
