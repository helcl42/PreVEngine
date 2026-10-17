#ifndef __WINDOW_COMMON_H__
#define __WINDOW_COMMON_H__

#include <string>

namespace prev::window {
// The window the engine opens. Auto opens the platform's own - on Linux X11 first, which runs everywhere through
// XWayland too, then Wayland - and Headless opens none. Xcb and Xlib open the same X11 window and differ only in
// the handle the surface is made from.
enum class WindowSystem {
    Auto,
    Headless,
    Win32,
    MacOS,
    IOS,
    Android,
    Emscripten,
    Wayland,
    Xcb,
    Xlib
};

struct WindowCreateInfo {
    WindowSystem windowSystem{ WindowSystem::Auto };

    std::string title{};

    int32_t left{};

    int32_t top{};

    uint32_t width{};

    uint32_t height{};

    bool fullScreen{};
};
} // namespace prev::window

#endif // !__WINDOW_COMMON_H__
