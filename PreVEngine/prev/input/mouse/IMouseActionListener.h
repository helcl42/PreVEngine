#ifndef __IMOUSE_ACTION_LISTENER_H__
#define __IMOUSE_ACTION_LISTENER_H__

#include "MouseEvents.h"

namespace prev::input::mouse {
class IMouseActionListener {
public:
    virtual void OnMouseAction(const MouseEvent& mouseAction) = 0;

public:
    virtual ~IMouseActionListener() = default;
};
} // namespace prev::input::mouse

#endif