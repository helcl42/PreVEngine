#include "GamepadBackendFactory.h"

#if defined(TARGET_PLATFORM_LINUX)
#include "impl/evdev/EvdevGamepadBackend.h"
#elif defined(TARGET_PLATFORM_WINDOWS)
#include "impl/windows/XInputGamepadBackend.h"
#elif defined(TARGET_PLATFORM_MACOS) || defined(TARGET_PLATFORM_IOS)
#include "impl/apple/GameControllerGamepadBackend.h"
#elif defined(TARGET_PLATFORM_EMSCRIPTEN)
#include "impl/emscripten/WebGamepadBackend.h"
#elif defined(TARGET_PLATFORM_ANDROID)
#include "impl/android/AndroidGamepadBackend.h"
#endif

namespace prev::input::gamepad {
std::unique_ptr<IGamepadBackend> GamepadBackendFactory::Create() const
{
#if defined(TARGET_PLATFORM_LINUX)
    return std::make_unique<impl::evdev::EvdevGamepadBackend>();
#elif defined(TARGET_PLATFORM_WINDOWS)
    return std::make_unique<impl::windows::XInputGamepadBackend>();
#elif defined(TARGET_PLATFORM_MACOS) || defined(TARGET_PLATFORM_IOS)
    return std::make_unique<impl::apple::GameControllerGamepadBackend>();
#elif defined(TARGET_PLATFORM_EMSCRIPTEN)
    return std::make_unique<impl::emscripten::WebGamepadBackend>();
#elif defined(TARGET_PLATFORM_ANDROID)
    return std::make_unique<impl::android::AndroidGamepadBackend>();
#else
    return nullptr;
#endif
}
} // namespace prev::input::gamepad
