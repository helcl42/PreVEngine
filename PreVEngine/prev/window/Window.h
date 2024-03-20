#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "../common/Common.h"
#include "../event/EventHandler.h"

#include "InputConvertor.h"

#include "AbstractWindow.h"

namespace prev::window {
class Window : public AbstractWindow {
public:
    Window(const WindowCreateInfo& createInfo);

    ~Window() = default;

public:
    virtual void OnInitEvent() override;

    virtual void OnCloseEvent() override;

    virtual void OnChangeEvent() override;

    virtual void OnResizeEvent(uint32_t width, uint32_t height) override;

    virtual void OnMoveEvent(int32_t x, int32_t y) override;

    virtual void OnFocusEvent(bool hasFocus) override;

    virtual void OnKeyEvent(impl::ActionType action, prev::input::keyboard::KeyCode keyCode) override;

    virtual void OnMouseEvent(impl::ActionType action, int32_t x, int32_t y, impl::ButtonType button, uint32_t w, uint32_t h) override;

    virtual void OnMouseScrollEvent(int32_t delta, int32_t x, int32_t y) override;

    virtual void OnTouchEvent(impl::ActionType action, float x, float y, uint8_t pointerId, float w, float h) override;

    virtual void OnTextEvent(const char* str) override;

public:
    void operator()(const prev::input::mouse::MouseLockRequest& mouseLock);

    void operator()(const prev::input::mouse::MouseCursorVisibilityRequest& cursorVisibility);

private:
    prev::event::EventHandler<Window, prev::input::mouse::MouseLockRequest> m_mouseLockHandler{ *this };

    prev::event::EventHandler<Window, prev::input::mouse::MouseCursorVisibilityRequest> m_mouseCursorVisibilityHandler{ *this };
};

} // namespace prev::window

#endif
