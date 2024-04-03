#ifdef VK_USE_PLATFORM_MACOS_MVK

#ifndef __MACOS_WINDOW_IMPL_H__
#define __MACOS_WINDOW_IMPL_H__

#include "../WindowImpl.h"

namespace prev::window::impl::macos {
struct MacState;

class MacOSWindowImpl final : public WindowImpl {
public:
    MacOSWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~MacOSWindowImpl();

public:
    Event GetEvent(bool waitForEvent = false);

private:
    void SetTitle(const std::string& title);

    void SetPosition(int32_t x, int32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    Surface& CreateSurface();

private:
    std::unique_ptr<MacState> m_state{};
};
} // namespace prev::window::impl::macos

#endif

#endif // VK_USE_PLATFORM_MACOS_MVK
