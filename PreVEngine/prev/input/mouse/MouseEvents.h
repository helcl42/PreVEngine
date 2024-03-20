#ifndef __MOUSE_EVENTS_H__
#define __MOUSE_EVENTS_H__

#include "../../common/Common.h"

namespace prev::input::mouse {
enum class MouseActionType : uint32_t {
    PRESS = 0,
    RELEASE = 1,
    MOVE = 2
};

enum class MouseButtonType : uint32_t {
    NONE = 0,
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3
};

struct MouseEvent {
    MouseActionType action{};

    MouseButtonType button{};

    glm::vec2 position;

    glm::vec2 extent;
};

struct MouseScrollEvent {
    int32_t delta{};

    glm::vec2 position;
};

struct MouseLockRequest {
    bool lock{};
};

struct MouseCursorVisibilityRequest {
    bool visible{};
};
} // namespace prev::input::mouse

#endif // !__MOUSE_EVENTS_H__
