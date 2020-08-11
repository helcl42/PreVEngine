#ifndef __TOUCH_EVENTS_H__
#define __TOUCH_EVENTS_H__

#include "../../common/Common.h"

namespace PreVEngine {
enum class TouchActionType {
    DOWN,
    UP,
    MOVE
};

struct TouchEvent {
    TouchActionType action;

    uint8_t pointerId;

    glm::vec2 position;

    glm::vec2 extent;
};
} // namespace PreVEngine

#endif