#ifndef __XINPUT_GAMEPAD_BACKEND_H__
#define __XINPUT_GAMEPAD_BACKEND_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_WINDOWS)

#include <array>
#include <chrono>

namespace prev::input::gamepad::impl::windows {
class XInputGamepadBackend final : public IGamepadBackend {
public:
    XInputGamepadBackend();

    ~XInputGamepadBackend() override;

    XInputGamepadBackend(const XInputGamepadBackend& other) = delete;

    XInputGamepadBackend& operator=(const XInputGamepadBackend& other) = delete;

    XInputGamepadBackend(XInputGamepadBackend&& other) = delete;

    XInputGamepadBackend& operator=(XInputGamepadBackend&& other) = delete;

public:
    void Poll(GamepadInputs& inputs) override;

private:
    using Clock = std::chrono::steady_clock;

    void* m_module{ nullptr }; // HMODULE

    void* m_getState{ nullptr }; // XInputGetState

    std::array<bool, MAX_GAMEPAD_COUNT> m_connected{};

    std::array<Clock::time_point, MAX_GAMEPAD_COUNT> m_nextProbe{};
};
} // namespace prev::input::gamepad::impl::windows

#endif

#endif // !__XINPUT_GAMEPAD_BACKEND_H__
