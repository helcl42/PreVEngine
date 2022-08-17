#ifndef __ABSTRACT_WINDOW_H__
#define __ABSTRACT_WINDOW_H__

#include "IWindow.h"
#include "WindowCommon.h"
#include "WindowEvents.h"

#include "impl/WindowImpl.h"

#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"

namespace prev::window {
class AbstractWindow : public IWindow {
public:
    AbstractWindow(const WindowCreateInfo& createInfo);

    virtual ~AbstractWindow() = default;

public:
    impl::Surface& GetSurface(VkInstance instance) override;

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const override;

    impl::Position GetPosition() const override;

    impl::Size GetSize() const override;

    bool IsKeyPressed(const prev::input::keyboard::KeyCode key) const override;

    bool IsMouseButtonPressed(const impl::ButtonType btn) const override;

    impl::Position GetMousePosition() const override;

    bool HasFocus() const override;

    bool IsMouseLocked() const override;

    bool IsMouseCursorVisible() const override;

    void SetTitle(const std::string& title) override;

    void SetPosition(const impl::Position& position) override;

    void SetSize(const impl::Size& size) override;

    void ShowKeyboard(bool enabled) override;

    void SetMouseLocked(bool locked) override;

    void SetMouseCursorVisible(bool visible) override;

    void Close() override;

public:
    impl::Event GetEvent(bool waitForEvent = false) override; // Return a single event from the queue (Alternative to using ProcessEvents.)

    bool ProcessEvents(bool waitForEvent = false) override; // Poll events, and call event handlers. Returns false if window is closing.

public:
    virtual void OnInitEvent() = 0;

    virtual void OnCloseEvent() = 0;

    virtual void OnChangeEvent() = 0;

    virtual void OnResizeEvent(uint16_t width, uint16_t height) = 0;

    virtual void OnMoveEvent(int16_t x, int16_t y) = 0;

    virtual void OnFocusEvent(bool hasFocus) = 0;

    virtual void OnMouseEvent(impl::ActionType action, int16_t x, int16_t y, impl::ButtonType btn, int16_t w, int16_t h) = 0;

    virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) = 0;

    virtual void OnKeyEvent(impl::ActionType action, prev::input::keyboard::KeyCode keycode) = 0;

    virtual void OnTextEvent(const char* str) = 0;

    virtual void OnTouchEvent(impl::ActionType action, float x, float y, uint8_t id, float w, float h) = 0;

private:
    std::shared_ptr<impl::WindowImpl> m_windowImpl;
};

} // namespace prev::window

#endif