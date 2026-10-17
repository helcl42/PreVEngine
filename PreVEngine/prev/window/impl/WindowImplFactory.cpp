#include "WindowImplFactory.h"

#include "../../common/Logger.h"
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
#ifdef TARGET_PLATFORM_EMSCRIPTEN
#include "emscripten/EmscriptenWindowImpl.h"
#endif

#include <stdexcept>
#include <vector>

namespace prev::window::impl {
namespace {
#if defined(GFX_HEADLESS_BUILD)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Headless }; // no windowing system compiled in
#elif defined(TARGET_PLATFORM_XCB) && defined(TARGET_PLATFORM_WAYLAND)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Xlib, prev::window::WindowSystem::Wayland };
#elif defined(TARGET_PLATFORM_XCB)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Xlib };
#elif defined(TARGET_PLATFORM_WAYLAND)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Wayland };
#elif defined(TARGET_PLATFORM_ANDROID)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Android };
#elif defined(TARGET_PLATFORM_WINDOWS)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Win32 };
#elif defined(TARGET_PLATFORM_MACOS)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::MacOS };
#elif defined(TARGET_PLATFORM_IOS)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::IOS };
#elif defined(TARGET_PLATFORM_EMSCRIPTEN)
    constexpr prev::window::WindowSystem AUTO_ORDER[]{ prev::window::WindowSystem::Emscripten };
#else
#error Not Implemented Window System!
#endif

    const char* Name(prev::window::WindowSystem system)
    {
        switch (system) {
        case prev::window::WindowSystem::Auto:
            return "auto";
        case prev::window::WindowSystem::Headless:
            return "headless";
        case prev::window::WindowSystem::Win32:
            return "Win32";
        case prev::window::WindowSystem::MacOS:
            return "macOS";
        case prev::window::WindowSystem::IOS:
            return "iOS";
        case prev::window::WindowSystem::Android:
            return "Android";
        case prev::window::WindowSystem::Emscripten:
            return "Emscripten";
        case prev::window::WindowSystem::Wayland:
            return "Wayland";
        case prev::window::WindowSystem::Xcb:
            return "XCB";
        case prev::window::WindowSystem::Xlib:
            return "Xlib";
        }
        return "?";
    }

    // Xcb and Xlib reach the same X server: when one cannot open, neither can the other.
    bool SameServer(prev::window::WindowSystem a, prev::window::WindowSystem b)
    {
        const auto isX11 = [](prev::window::WindowSystem system) {
            return system == prev::window::WindowSystem::Xcb || system == prev::window::WindowSystem::Xlib;
        };
        return a == b || (isX11(a) && isX11(b));
    }

    // The requested system first, then the platform's own in Auto's order, so a choice that cannot open still
    // gets a window. Headless is the exception: asking for no window never falls back to one.
    std::vector<prev::window::WindowSystem> Candidates(prev::window::WindowSystem requested)
    {
        if (requested == prev::window::WindowSystem::Headless) {
            return { prev::window::WindowSystem::Headless };
        }
        std::vector<prev::window::WindowSystem> candidates;
        if (requested != prev::window::WindowSystem::Auto) {
            candidates.push_back(requested);
        }
        for (const auto system : AUTO_ORDER) {
            if (!SameServer(system, requested)) {
                candidates.push_back(system);
            }
        }
        return candidates;
    }

    std::unique_ptr<WindowImpl> TryCreate(const WindowInfo& info)
    {
        switch (info.windowSystem) {
        case prev::window::WindowSystem::Headless:
            return std::make_unique<headless::HeadlessWindowImpl>(info);
#if defined(TARGET_PLATFORM_ANDROID) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::Android:
            return std::make_unique<android::AndroidWindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_XCB) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::Xcb:
        case prev::window::WindowSystem::Xlib:
            return std::make_unique<xcb::XcbWindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_WAYLAND) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::Wayland:
            return std::make_unique<wayland::WaylandWindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_WINDOWS) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::Win32:
            return std::make_unique<win32::Win32WindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_MACOS) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::MacOS:
            return std::make_unique<macos::MacOSWindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_IOS) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::IOS:
            return std::make_unique<ios::IOSWindowImpl>(info);
#endif
#if defined(TARGET_PLATFORM_EMSCRIPTEN) && !defined(GFX_HEADLESS_BUILD)
        case prev::window::WindowSystem::Emscripten:
            return std::make_unique<emscripten::EmscriptenWindowImpl>(info);
#endif
        default:
            return nullptr;
        }
    }
} // namespace

std::unique_ptr<WindowImpl> WindowImplFactory::Create(const WindowInfo& info) const
{
    for (const auto system : Candidates(info.windowSystem)) {
        WindowInfo attempt{ info };
        attempt.windowSystem = system;
        try {
            if (auto window{ TryCreate(attempt) }) {
                LOGI("Window opened on %s", Name(system));
                return window;
            }
            LOGW("%s window is not built in", Name(system));
        } catch (const std::exception& e) {
            LOGW("%s window could not open: %s", Name(system), e.what());
        }
    }
    throw std::runtime_error("No window system could open a window.");
}
} // namespace prev::window::impl
