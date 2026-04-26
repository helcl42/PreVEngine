#include "WindowImplFactory.h"

#include "headless/HeadlessWindowImpl.h"
#ifdef TARGET_PLATFORM_ANDROID
#include "android/AndroidWindowImpl.h"
#endif
#ifdef TARGET_PLATFORM_XCB
#include "xcb/XcbWindowImpl.h"
#endif
#ifdef TARGET_PLATFORM_WAYLAND
#include "wayland/WaylandWindowImpl.h"
#endif
#ifdef TARGET_PLATFORM_WINDOWS
#include "windows/Win32WindowImpl.h"
#endif
#ifdef TARGET_PLATFORM_MACOS
#include "macos/MacOSWindowImpl.h"
#endif
#ifdef TARGET_PLATFORM_IOS
#include "ios/IOSWindowImpl.h"
#endif

namespace prev::window::impl {
std::unique_ptr<WindowImpl> WindowImplFactory::Create(const WindowInfo& info) const
{
    if (info.headless) {
        return std::make_unique<headless::HeadlessWindowImpl>(info);
    }

#ifdef TARGET_PLATFORM_ANDROID
    return std::make_unique<android::AndroidWindowImpl>(info);
#elif defined(TARGET_PLATFORM_XCB)
    return std::make_unique<xcb::XcbWindowImpl>(info);
#elif defined(TARGET_PLATFORM_WAYLAND)
    return std::make_unique<wayland::WaylandWindowImpl>(info);
#elif defined(TARGET_PLATFORM_WINDOWS)
    return std::make_unique<win32::Win32WindowImpl>(info);
#elif defined(TARGET_PLATFORM_MACOS)
    return std::make_unique<macos::MacOSWindowImpl>(info);
#elif defined(TARGET_PLATFORM_IOS)
    return std::make_unique<ios::IOSWindowImpl>(info);
#else
#error Not Implemented Window System!
#endif
}
} // namespace prev::window::impl
