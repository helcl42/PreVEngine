#ifndef __WINDOW_EVENTS_H__
#define __WINDOW_EVENTS_H__

#include "IWindow.h"

#include "../common/Common.h"
#include "../core/instance/Validation.h"

#include <inttypes.h>

namespace PreVEngine {
struct WindowCreatedEvent {
    IWindow* window;
};

struct WindowDestroyedEvent {
    IWindow* window;
};

struct WindowChangeEvent {
    IWindow* window;
};

struct WindowResizeEvent {
    IWindow* window;

    uint32_t width;

    uint32_t height;
};

struct WindowMovedEvent {
    IWindow* window;

    glm::vec2 position;
};

struct WindowFocusChangeEvent {
    IWindow* window;

    bool hasFocus;
};

struct SurfaceChanged {
    VkSurfaceKHR surface;
};
} // namespace PreVEngine

#endif