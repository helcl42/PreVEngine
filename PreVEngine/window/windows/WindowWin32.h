#ifdef VK_USE_PLATFORM_WIN32_KHR

#ifndef __WINDOW_WIN32__
#define __WINDOW_WIN32__

#include "../WindowImpl.h"

#include <windowsx.h> // Mouse

namespace prev::window::windows {
class WindowWin32 final : public WindowImpl {
public:
    WindowWin32(const char* title); // creates fullScreen window

    WindowWin32(const char* title, uint32_t width, uint32_t height);

    ~WindowWin32();

public:
    Event GetEvent(bool waitForEvent = false);

    bool CanPresent(VkPhysicalDevice phy, uint32_t queueFamily) const;

private:
    void SetTitle(const char* title);

    void SetPosition(uint32_t x, uint32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    bool CreateSurface(VkInstance instance);

    void GetDesktopResolution(uint32_t& horizontal, uint32_t& vertical) const;

    void Init(const char* title, uint32_t width, uint32_t height, bool isFullScreen);

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE m_hInstance;

    HWND m_hWnd;

    MultiTouch m_MTouch; // Multi-Touch device
};

#endif
}

#endif // VK_USE_PLATFORM_WIN32_KHR
