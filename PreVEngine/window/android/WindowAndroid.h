#ifdef VK_USE_PLATFORM_ANDROID_KHR

#ifndef __WINDOW_ANDROID__
#define __WINDOW_ANDROID__

#include "../WindowImpl.h"
#include "native.h" // for Android_App

namespace prev::window::android {
class WindowAndroid : public WindowImpl {
public:
    WindowAndroid(const char* title, uint32_t width, uint32_t height);
    
    ~WindowAndroid();

    Event GetEvent(bool waitForEvent = false);

    void SetTextInput(bool enabled);

private:
    void SetTitle(const char* title);

    void SetPosition(uint32_t x, uint32_t y);

    void SetSize(uint32_t w, uint32_t h);

    void SetMouseCursorVisible(bool visible);

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const;

    bool CreateSurface(VkInstance instance);

private:
    android_app* m_app{ nullptr };

    MultiTouch m_MTouch;
};
} // namespace prev::window::android

#endif

#endif // VK_USE_PLATFORM_ANDROID_KHR
