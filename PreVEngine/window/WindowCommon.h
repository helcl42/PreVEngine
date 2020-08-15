#ifndef __WINDOW_COMMON_H__
#define __WINDOW_COMMON_H__

#include <inttypes.h>

namespace prev {
struct Position {
    int16_t x;

    int16_t y;
};

struct Size {
    uint16_t width;

    uint16_t height;
};

enum class ActionType // keyboard / mouse / touchscreen actions
{
    UP,
    DOWN,
    MOVE
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
            int16_t x;
            int16_t y;
            ButtonType btn;
            int16_t w;
            int16_t h;
        } mouse;

        struct // mouse scroll
        {
            int16_t delta;
            int16_t x;
            int16_t y;
        } scroll;

        struct // Keyboard key state
        {
            ActionType action;
            KeyCode keycode;
        } key;

        struct // Text entered
        {
            const char* str;
        } text;

        struct // Window move
        {
            int16_t x;
            int16_t y;
        } move;

        struct // Window resize
        {
            uint16_t width;
            uint16_t height;
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
} // namespace prev

#endif