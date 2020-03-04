#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef ANDROID
#include <native.h>
#endif

#include "Common.h"
#include "Events.h"
#include "EventsChannel.h"
#include "IWindow.h"
#include "WindowImpl.h"

namespace PreVEngine {
class AbstractWindow : public IWindow {
private:
    std::shared_ptr<WindowImpl> m_windowImpl;

private:
    void InitWindow(const char* title, const uint32_t width, const uint32_t height, bool tryFullscreen);

public:
    AbstractWindow(const char* title);

    AbstractWindow(const char* title, const uint32_t width, const uint32_t height);

    virtual ~AbstractWindow() = default; 

public:
    Surface& GetSurface(VkInstance instance);

    bool CanPresent(VkPhysicalDevice gpu, uint32_t queueFamily) const;

public:
    Position GetPosition() const;

    Size GetSize() const;

    bool IsKeyPressed(const KeyCode key) const;

    bool IsMouseButtonPressed(const ButtonType btn) const;

    Position GetMousePosition() const;

    bool HasFocus() const;

    bool IsMouseLocked() const;

    bool IsMouseCursorVisible() const;

public:
    void SetTitle(const char* title);

    void SetPosition(const Position& position);

    void SetSize(const Size& size);

    void ShowKeyboard(bool enabled);

    void SetMouseLocked(bool locked);

    void SetMouseCursorVisible(bool visible);

    void Close();

public:
    Event GetEvent(bool waitForEvent = false); // Return a single event from the queue (Alternative to using ProcessEvents.)

    bool ProcessEvents(bool waitForEvent = false); // Poll events, and call event handlers. Returns false if window is closing.

public:
    virtual void OnInitEvent() = 0;

    virtual void OnCloseEvent() = 0;

    virtual void OnChangeEvent() = 0;

    virtual void OnResizeEvent(uint16_t width, uint16_t height) = 0;

    virtual void OnMoveEvent(int16_t x, int16_t y) = 0;

    virtual void OnFocusEvent(bool hasFocus) = 0;

    virtual void OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType btn, int16_t w, int16_t h) = 0;

    virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) = 0;

    virtual void OnKeyEvent(ActionType action, KeyCode keycode) = 0;

    virtual void OnTextEvent(const char* str) = 0;

    virtual void OnTouchEvent(ActionType action, float x, float y, uint8_t id, float w, float h) = 0;
};

class Window : public AbstractWindow {
private:
    EventHandler<Window, MouseLockRequest> m_mouseLockHandler{ *this };

    EventHandler<Window, MouseCursorVisibilityRequest> m_mouseCursorVisibilityHandler{ *this };

public:
    Window(const char* title)
        : AbstractWindow(title)
    {
    }

    Window(const char* title, const uint32_t width, const uint32_t height)
        : AbstractWindow(title, width, height)
    {
    }

public:
    virtual void OnInitEvent() override
    {
        EventChannel::Broadcast(WindowCreatedEvent{ this });
    }

    virtual void OnCloseEvent() override
    {
        EventChannel::Broadcast(WindowDestroyedEvent{ this });
    }

    virtual void OnChangeEvent() override
    {
        EventChannel::Broadcast(WindowChangeEvent{ this });
    }

    virtual void OnResizeEvent(uint16_t width, uint16_t height) override
    {
        EventChannel::Broadcast(WindowResizeEvent{ this, width, height });
    }

    virtual void OnMoveEvent(int16_t x, int16_t y) override
    {
        EventChannel::Broadcast(WindowMovedEvent{ this, glm::vec2(x, y) });
    }

    virtual void OnFocusEvent(bool hasFocus) override
    {
        EventChannel::Broadcast(WindowFocusChangeEvent{ this, hasFocus });
    }

    virtual void OnKeyEvent(ActionType action, KeyCode keyCode) override
    {
        EventChannel::Broadcast(KeyEvent{ InputsMapping::GetKeyActionType(action), keyCode });
    }

    virtual void OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType button, int16_t w, int16_t h) override
    {
        EventChannel::Broadcast(MouseEvent{ InputsMapping::GetMouseActionType(action), InputsMapping::GetMouseButtonType(button), glm::vec2(x, y), glm::vec2(w, h) });
    }

    virtual void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) override
    {
        EventChannel::Broadcast(MouseScrollEvent{ delta, glm::vec2(x, y) });
    }

    virtual void OnTouchEvent(ActionType action, float x, float y, uint8_t pointerId, float w, float h) override
    {
        EventChannel::Broadcast(TouchEvent{ InputsMapping::GetTouchActionType(action), pointerId, glm::vec2(x, y), glm::vec2(w, h) });
    }

    virtual void OnTextEvent(const char* str) override
    {
        EventChannel::Broadcast(TextEvent{ str });
    }

public:
    void operator()(const MouseLockRequest& mouseLock)
    {
        SetMouseLocked(mouseLock.lock);
    }

    void operator()(const MouseCursorVisibilityRequest& cursorVisibility)
    {
        SetMouseCursorVisible(cursorVisibility.visible);
    }
};

} // namespace PreVEngine

#endif
