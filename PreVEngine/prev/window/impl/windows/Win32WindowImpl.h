#ifdef VK_USE_PLATFORM_WIN32_KHR

#ifndef __WIN32_WINDOW_IMPL_H__
#define __WIN32_WINDOW_IMPL_H__

#include "../WindowImpl.h"

namespace prev::window::impl::win32 {
class Win32WindowImpl final : public WindowImpl {
public:
    Win32WindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~Win32WindowImpl();

public:
    bool PollEvent(bool waitForEvent, Event& outEvent) override;

    void SetTitle(const std::string& title) override;

    void SetPosition(int32_t x, int32_t y) override;

    void SetSize(uint32_t w, uint32_t h) override;

    void SetMouseCursorVisible(bool visible) override;

    Surface& CreateSurface() override;

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE m_hInstance{};

    HWND m_hWnd{};

    MultiTouch m_MTouch{}; // Multi-Touch device

    char m_textBuffer[4] = {}; // store char for text event
};
} // namespace prev::window::impl::win32

#endif

#endif // VK_USE_PLATFORM_WIN32_KHR
