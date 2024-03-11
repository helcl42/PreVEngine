#ifdef VK_USE_PLATFORM_MACOS_MVK

#ifndef __MACOS_WINDOW_IMPL__
#define __MACOS_WINDOW_IMPL__

#include "../WindowImpl.h"

namespace prev::window::impl::macos {
class MacOSWindowImpl final : public WindowImpl {
public:
    MacOSWindowImpl(const WindowInfo& windowInfo);

    ~MacOSWindowImpl();

public:
    Event GetEvent(bool waitForEvent = false);

    bool CanPresent(VkPhysicalDevice phy, uint32_t queueFamily) const; // check if this window can present this queue type

private:
    void SetTitle(const std::string& title);

    void SetPosition(int32_t x, int32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    bool CreateSurface(VkInstance instance);
};
} // namespace prev::window::impl::macos

#endif

#endif // VK_USE_PLATFORM_MACOS_MVK
