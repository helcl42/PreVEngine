#ifndef __ABSTRACT_WINDOW_H__
#define __ABSTRACT_WINDOW_H__

#include "../input/keyboard/KeyboardEvents.h"
#include "../input/mouse/MouseEvents.h"
#include "../input/touch/TouchEvents.h"
#include "WindowEvents.h"

#include "IWindow.h"
#include "impl/WindowImpl.h"

namespace prev::window {
class AbstractWindow : public IWindow {
public:
    AbstractWindow(const char* title);

    AbstractWindow(const char* title, const uint32_t width, const uint32_t height);

    virtual ~AbstractWindow() = default;

public:
    impl::Surface& GetSurface(VkInstance instance) override;

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const override;

    Position GetPosition() const override;

    Size GetSize() const override;

    bool IsKeyPressed(const prev::input::keyboard::KeyCode key) const override;

    bool IsMouseButtonPressed(const ButtonType btn) const override;

    Position GetMousePosition() const override;

    bool HasFocus() const override;

    bool IsMouseLocked() const override;

    bool IsMouseCursorVisible() const override;

    void SetTitle(const char* title) override;

    void SetPosition(const Position& position) override;

    void SetSize(const Size& size) override;

    void ShowKeyboard(bool enabled) override;

    void SetMouseLocked(bool locked) override;

    void SetMouseCursorVisible(bool visible) override;

    void Close() override;

public:
    Event GetEvent(bool waitForEvent = false) override; // Return a single event from the queue (Alternative to using ProcessEvents.)

    bool ProcessEvents(bool waitForEvent = false) override; // Poll events, and call event handlers. Returns false if window is closing.

public:
    virtual void OnInitEvent() = 0;

    virtual void OnCloseEvent() = 0;

    virtual void OnChangeEvent() = 0;

    virtual void OnResizeEvent(uint16_t width, uint16_t height) = 0;

    virtual void OnMoveEvent(int16_t x, int16_t y) = 0;

    virtual void OnFocusEvent(bool hasFocus) = 0;

    virtual void OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType btn, int16_t w, int16_t h) = 0;

    virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) = 0;

    virtual void OnKeyEvent(ActionType action, prev::input::keyboard::KeyCode keycode) = 0;

    virtual void OnTextEvent(const char* str) = 0;

    virtual void OnTouchEvent(ActionType action, float x, float y, uint8_t id, float w, float h) = 0;

private:
    void InitWindow(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen);

private:
    std::shared_ptr<impl::WindowImpl> m_windowImpl;
};

}

#endif