#ifdef VK_USE_PLATFORM_ANDROID_KHR

#ifndef __ANDROID_WINDOW_IMPL__
#define __ANDROID_WINDOW_IMPL__

#include "../WindowImpl.h"

#include <android_native.h> // for Android_App

namespace prev::window::impl::android {
class AndroidWindowImpl : public WindowImpl {
public:
    AndroidWindowImpl(const prev::core::instance::Instance& instance, const WindowInfo& windowInfo);

    ~AndroidWindowImpl();

public:
    Event GetEvent(bool waitForEvent = false);

    void SetTextInput(bool enabled);

private:
    void SetTitle(const std::string& title);

    void SetPosition(int32_t x, int32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    Surface& CreateSurface();

private:
    android_app* m_app{};

    MultiTouch m_MTouch;

    char m_textBuffer[4] = {}; // store char for text event
};
} // namespace prev::window::impl::android

#endif

#endif // VK_USE_PLATFORM_ANDROID_KHR
