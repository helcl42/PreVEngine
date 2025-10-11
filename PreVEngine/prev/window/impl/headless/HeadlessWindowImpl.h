#ifndef __HEADLESS_WINDOW_IMPL_H__
#define __HEADLESS_WINDOW_IMPL_H__

#include "../WindowImpl.h"

namespace prev::window::impl::headless {
class HeadlessWindowImpl final : public WindowImpl {
public:
    HeadlessWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~HeadlessWindowImpl();

public:
    bool PollEvent(bool waitForEvent, Event& outEvent) override;

    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

    Surface& CreateSurface() override;
};
} // namespace prev::window::impl::headless

#endif // !__HEADLESS_WINDOW_IMPL_H__