#ifndef __WINDOW_IMPL_COMMON_H__
#define __WINDOW_IMPL_COMMON_H__

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
            const char* str;
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
    };

    void Clear()
    {
        tag = EventType::NONE;
    }
};
} // namespace prev::window::impl

#endif