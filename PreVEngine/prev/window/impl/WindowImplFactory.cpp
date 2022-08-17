#include "WindowImplFactory.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include "android/AndroidWindowImpl.h"
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
#include "linux/XcbWindowImpl.h"
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
#include "windows/Win32WindowImpl.h"
#endif

namespace prev::window::impl {
std::unique_ptr<WindowImpl> WindowImplFactory::Create(const WindowInfo& info) const
{
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#elif VK_USE_PLATFORM_XCB_KHR
#elif VK_USE_PLATFORM_WIN32_KHR
    return std::make_unique<windows::Win32WindowImpl>(info);
#else
#error NOT IMPLEMENTED PLATFORM !
#endif
    // TODO:
    //    #ifdef VK_USE_PLATFORM_XLIB_KHR
    //    #ifdef VK_USE_PLATFORM_MIR_KHR
    //    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
}
} // namespace prev::window::impl