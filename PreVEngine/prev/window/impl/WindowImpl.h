#ifndef __WINDOW_IMPL_H__
#define __WINDOW_IMPL_H__

#include "Surface.h"
#include "WindowImplCommon.h"

#include "../../core/instance/Instance.h"

#include "../../util/Utils.h"

namespace prev::window::impl {
class MultiTouch {
public:
    struct Pointer {
        bool active{};

        float x{};

        float y{};
    };

public:
    MultiTouch()
        : m_count{ MAX_POINTER_COUNT }
    {
        Clear();
    }

public:
    void Clear()
    {
        for (int i = 0; i < MAX_POINTER_COUNT; ++i) {
            m_touchID[i] = 0;
            m_pointers[i] = {};
        }
    }

    int GetCount() const
    {
        return m_count;
    }

    void SetCount(int cnt)
    {
        m_count = cnt;
    }

    // Convert desktop-style touch-id's to an android-style finger-id.
    Event OnEventById(ActionType action, float x, float y, uint32_t id, uint32_t value, float w, float h)
    {
        for (uint32_t i = 0; i < MAX_POINTER_COUNT; ++i) {
            if (m_touchID[i] == id) { // lookup finger-id
                m_touchID[i] = value;
                return OnEvent(action, x, y, i, w, h);
            }
        }
        return { Event::EventType::UNKNOWN };
    }

    Event OnEvent(ActionType action, float x, float y, uint32_t id, float w, float h)
    {
        if (id >= MAX_POINTER_COUNT) {
            return Event{}; // Exit if too many fingers
        }

        Pointer& P = m_pointers[id];
        if (action != ActionType::MOVE) {
            P.active = (action == ActionType::DOWN);
        }

        P.x = x;
        P.y = y;

        Event e = { Event::EventType::TOUCH };
        e.touch = { action, x, y, static_cast<uint8_t>(id), w, h };
        return e;
    }

    Pointer GetPointer(uint32_t id) const
    {
        if (id >= MAX_POINTER_COUNT) {
            return {};
        }

        return m_pointers[id];
    }

private:
    static const inline uint32_t MAX_POINTER_COUNT{ 10 }; // Max 10 fingers

private:
    uint32_t m_touchID[MAX_POINTER_COUNT] = {}; // finger-id lookup table (Desktop)

    Pointer m_pointers[MAX_POINTER_COUNT] = {};

    int m_count{}; // number of active touch-id's
};

class WindowImpl : public Surface {
public:
    WindowImpl(const prev::core::instance::Instance& instance);

    virtual ~WindowImpl() = default;

public:
    bool IsKeyPressed(const prev::input::keyboard::KeyCode key) const;

    bool IsMouseButtonPressed(const ButtonType btn) const;

    Position GetMousePosition() const;

    bool HasFocus() const;

    bool IsRunning() const;

    const WindowInfo& GetInfo() const;

    bool IsMouseLocked() const;

    void SetMouseLocked(bool locked);

    bool IsMouseCursorVisible() const;

    void DestroySurface();

public:
    virtual void SetTextInput(bool enabled); // Shows the Android soft-keyboard. //TODO: Enable OnTextEvent?

    virtual bool HasTextInput() const;

    virtual void Close();

public:
    virtual bool PollEvent(bool waitForEvent, Event& outEvent) = 0;

    virtual void SetTitle(const std::string& title) = 0;

    virtual void SetPosition(int32_t x, int32_t y) = 0;

    virtual void SetSize(uint32_t w, uint32_t h) = 0;

    virtual void SetMouseCursorVisible(bool visible) = 0;

    virtual Surface& CreateSurface() = 0;

protected:
    Event OnMouseEvent(ActionType action, int32_t x, int32_t y, ButtonType btn); // Mouse event

    Event OnMouseScrollEvent(int32_t delta, int32_t x, int32_t y);

    Event OnKeyEvent(ActionType action, uint8_t key); // Keyboard event

    Event OnTextEvent(uint32_t unicode); // Text event

    Event OnMoveEvent(int32_t x, int32_t y); // Window moved

    Event OnResizeEvent(uint32_t width, uint32_t height); // Window resized

    Event OnFocusEvent(bool hasFocus); // Window gained/lost focus

    Event OnInitEvent(); // Window was initialized

    Event OnCloseEvent(); // Window closing

    Event OnChangeEvent();

protected:
    const prev::core::instance::Instance& m_instance;

    prev::util::CircularQueue<Event, 32> m_eventQueue{};

    bool m_running{};

    bool m_hasTextInput{};

    bool m_hasFocus{};

    WindowInfo m_info{};

    bool m_mouseLocked{};

    bool m_mouseCursorVisible{};

private:
    Position m_mousePosition{};

    bool m_mouseButtonsState[4] = {};

    bool m_keyboardKeysState[256] = {};
};
} // namespace prev::window::impl

#endif
