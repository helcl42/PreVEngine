#ifdef VK_USE_PLATFORM_IOS_MVK

#ifndef __IOS_WINDOW_IMPL_H__
#define __IOS_WINDOW_IMPL_H__

#include "../WindowImpl.h"

namespace prev::window::impl::ios {
struct IOSState;

class IOSWindowImpl final : public WindowImpl {
public:
    IOSWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~IOSWindowImpl();

public:
    Surface& CreateSurface() override;
    
    void PollEvents(bool waitForEvent = false) override;
    
    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

private:
    std::unique_ptr<IOSState> m_state{};
    
    MultiTouch m_MTouch{}; // Multi-Touch device
};
} // namespace prev::window::impl::ios

#endif

#endif // VK_USE_PLATFORM_IOS_MVK
