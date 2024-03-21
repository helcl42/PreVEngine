#include "WindowImplFactory.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include "android/AndroidWindowImpl.h"
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
#include "xcb/XcbWindowImpl.h"
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
#include "windows/Win32WindowImpl.h"
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
#include "macos/MacOSWindowImpl.h"
#endif

namespace prev::window::impl {
std::unique_ptr<WindowImpl> WindowImplFactory::Create(const prev::core::instance::Instance& instance, const WindowInfo& info) const
{
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    return std::make_unique<android::AndroidWindowImpl>(instance, info);
#elif VK_USE_PLATFORM_XCB_KHR
    return std::make_unique<xcb::XcbWindowImpl>(instance, info);
#elif VK_USE_PLATFORM_WIN32_KHR
    return std::make_unique<win32::Win32WindowImpl>(instance, info);
#elif VK_USE_PLATFORM_MACOS_MVK
    return std::make_unique<macos::MacOSWindowImpl>(instance, info);
#else
#error NOT IMPLEMENTED PLATFORM!
#endif

    // TODO:
    // #ifdef VK_USE_PLATFORM_XLIB_KHR
    // #ifdef VK_USE_PLATFORM_MIR_KHR
    // #ifdef VK_USE_PLATFORM_WAYLAND_KHR
}
} // namespace prev::window::impl