#ifndef __IMOUSE_SCROLL_LISTENER_H__
#define __IMOUSE_SCROLL_LISTENER_H__

#include "MouseEvents.h"

namespace PreVEngine {
class IMouseScrollListener {
public:
    virtual void OnMouseScroll(const MouseScrollEvent& scroll) = 0;

public:
    virtual ~IMouseScrollListener() = default;
};
} // namespace PreVEngine

#endif // !__IMOUSE_SCROLL_LISTENER_H__
