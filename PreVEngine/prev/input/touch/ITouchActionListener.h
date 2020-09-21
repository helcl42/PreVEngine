#ifndef __ITOUCH_ACTION_LISTENER_H__
#define __ITOUCH_ACTION_LISTENER_H__

#include "TouchEvents.h"

namespace prev::input::touch {
class ITouchActionListener {
public:
    virtual void OnTouchAction(const TouchEvent& textEvent) = 0;

public:
    virtual ~ITouchActionListener() = default;
};

} // namespace prev::input::touch

#endif