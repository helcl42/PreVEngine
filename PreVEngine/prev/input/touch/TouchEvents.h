#ifndef __TOUCH_EVENTS_H__
#define __TOUCH_EVENTS_H__

#include "../../common/Common.h"

namespace prev::input::touch {
enum class TouchActionType {
    DOWN = 0,
    UP = 1,
    MOVE = 2
};

struct TouchEvent {
    TouchActionType action{};

    uint8_t pointerId{};

    glm::vec2 position;

    glm::vec2 extent;
};
} // namespace prev::input::touch

#endif