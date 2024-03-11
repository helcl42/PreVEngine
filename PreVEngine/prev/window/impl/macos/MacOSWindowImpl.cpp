#include "MacOSWindowImpl.h"

#ifdef VK_USE_PLATFORM_MACOS_MVK

namespace prev::window::impl::macos {
MacOSWindowImpl::MacOSWindowImpl(const WindowInfo& windowInfo)
{

}

MacOSWindowImpl::~MacOSWindowImpl()
{

}

Event MacOSWindowImpl::GetEvent(bool waitForEvent)
{
    return {};
}

bool MacOSWindowImpl::CanPresent(VkPhysicalDevice phy, uint32_t queueFamily) const
{
    return false;
}

void MacOSWindowImpl::SetTitle(const std::string& title)
{

}

void MacOSWindowImpl::SetPosition(int32_t x, int32_t y)
{

}

void MacOSWindowImpl::SetSize(uint32_t w, uint32_t h)
{

}

void MacOSWindowImpl::SetMouseCursorVisible(bool visible)
{

}

bool MacOSWindowImpl::CreateSurface(VkInstance instance)
{
    return false;
}
}

#endif