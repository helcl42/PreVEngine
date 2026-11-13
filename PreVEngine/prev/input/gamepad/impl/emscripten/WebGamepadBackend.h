#ifndef __WEB_GAMEPAD_BACKEND_H__
#define __WEB_GAMEPAD_BACKEND_H__

#include "../../IGamepadBackend.h"

#if defined(TARGET_PLATFORM_EMSCRIPTEN)

#include <array>

namespace prev::input::gamepad::impl::emscripten {
class WebGamepadBackend final : public IGamepadBackend {
public:
    WebGamepadBackend() = default;

    ~WebGamepadBackend() override = default;

public:
    void Poll(GamepadInputs& inputs) override;

private:
    std::array<int, MAX_GAMEPAD_COUNT> m_slotIndices{ -1, -1, -1, -1 };

    bool m_warnedNonStandard{ false };
};
} // namespace prev::input::gamepad::impl::emscripten

#endif

#endif // !__WEB_GAMEPAD_BACKEND_H__
