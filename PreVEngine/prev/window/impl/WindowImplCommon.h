#ifndef __WINDOW_IMPL_COMMON_H__
#define __WINDOW_IMPL_COMMON_H__

#include <algorithm>
#include <cinttypes>
#include <string>

#include "../../input/keyboard/KeyCodes.h"

namespace prev::window::impl {
struct Position {
    int32_t x{};

    int32_t y{};
};

inline bool operator==(const Position& a, const Position& b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Position& a, const Position& b)
{
    return !(a == b);
}

struct Size {
    uint32_t width{};

    uint32_t height{};
};

inline bool operator==(const Size& a, const Size& b)
{
    return a.width == b.width && a.height == b.height;
}

inline bool operator!=(const Size& a, const Size& b)
{
    return !(a == b);
}

struct WindowInfo {
    std::string title;

    Position position;

    Size size;

    bool fullScreen{};
};

enum class ActionType // keyboard / mouse / touchscreen actions
{
    UP = 0,
    DOWN = 1,
    MOVE = 2
};

enum class ButtonType {
    NONE = 0,
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3
};

struct Event {
    enum class EventType // event type
    {
        NONE,
        MOUSE,
        MOUSE_SCROLL,
        KEY,
        TEXT,
        MOVE,
        RESIZE,
        FOCUS,
        TOUCH,
        CLOSE,
        INIT,
        CHANGE,
        UNKNOWN
    };

    Event()
        : tag{ EventType::UNKNOWN }
        , body{}
    {
    }

    Event(EventType t)
        : tag{ t }
        , body{}
    {
    }

    EventType tag;

    union {
        struct // mouse move/click
        {
            ActionType action;
            int32_t x;
            int32_t y;
            ButtonType btn;
            uint32_t w;
            uint32_t h;
        } mouse;

        struct // mouse scroll
        {
            int32_t delta;
            int32_t x;
            int32_t y;
        } scroll;

        struct // Keyboard key state
        {
            ActionType action;
            prev::input::keyboard::KeyCode keyCode;
        } key;

        struct // Text entered
        {
            uint32_t unicode;
        } text;

        struct // Window move
        {
            int32_t x;
            int32_t y;
        } move;

        struct // Window resize
        {
            uint32_t width;
            uint32_t height;
        } resize;

        struct // Window gained/lost focus
        {
            bool hasFocus;
        } focus;

        struct // multi-touch display
        {
            ActionType action;
            float x;
            float y;
            uint8_t id;
            float w;
            float h;
        } touch;

        struct
        {
        } init; // window is ready

        struct
        {
        } close; // Window is closing

        struct
        {

        } change;
    } body;

    void Clear()
    {
        tag = EventType::NONE;
    }
};

class MultiTouch {
public:
    struct Pointer {
        bool active{};
        float x{};
        float y{};
    };

public:
    MultiTouch()
        : m_maxCount{ MAX_POINTER_COUNT }
    {
        Reset();
    }

public:
    void Reset()
    {
        for (uint32_t i = 0; i < MAX_POINTER_COUNT; ++i) {
            m_touchID[i] = 0;
            m_pointers[i] = {};
        }
    }

    uint32_t GetMaxCount() const
    {
        return m_maxCount;
    }

    void SetMaxCount(uint32_t maxCount)
    {
        m_maxCount = std::min(maxCount, MAX_POINTER_COUNT);
    }

    Event OnEventById(ActionType action, float x, float y, uint32_t id, uint32_t value, float w, float h)
    {
        for (uint32_t i = 0; i < m_maxCount; ++i) {
            if (m_touchID[i] == id) { // lookup finger-id
                m_touchID[i] = value;
                return OnEvent(action, x, y, i, w, h);
            }
        }
        return { Event::EventType::UNKNOWN };
    }

    Event OnEvent(ActionType action, float x, float y, uint32_t id, float w, float h)
    {
        if (id >= m_maxCount) {
            return {}; // Exit if too many fingers
        }

        Pointer& pointer = m_pointers[id];
        if (action != ActionType::MOVE) {
            pointer.active = (action == ActionType::DOWN);
        }

        pointer.x = x;
        pointer.y = y;

        Event e{ Event::EventType::TOUCH };
        e.body.touch = { action, x, y, static_cast<uint8_t>(id), w, h };
        return e;
    }

    Pointer GetPointer(uint32_t id) const
    {
        if (id >= m_maxCount) {
            return {};
        }

        return m_pointers[id];
    }

private:
    static const inline uint32_t MAX_POINTER_COUNT{ 10 }; // Max 10 fingers

private:
    uint32_t m_touchID[MAX_POINTER_COUNT] = {}; // finger-id lookup table (Desktop)

    Pointer m_pointers[MAX_POINTER_COUNT] = {};

    uint32_t m_maxCount{}; // number of active touch-id's
};

} // namespace prev::window::impl

#endif
