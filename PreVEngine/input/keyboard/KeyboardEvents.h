#ifndef __KEYBOARD_EVENTS_H__
#define __KEYBOARD_EVENTS_H__

#include "../../common/Common.h"
#include "KeyCodes.h"

namespace PreVEngine {
enum class KeyActionType : uint32_t {
    PRESS,
    RELEASE
};

struct KeyEvent {
    KeyActionType action;

    KeyCode keyCode;
};

struct TextEvent {
    std::string text;
};
} // namespace PreVEngine

#endif