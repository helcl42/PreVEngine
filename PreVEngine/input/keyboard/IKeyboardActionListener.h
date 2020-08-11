#ifndef __IKEYBOARD_ACTION_LISTENER_H__
#define __IKEYBOARD_ACTION_LISTENER_H__

#include "KeyboardEvents.h"

namespace PreVEngine {
class IKeyboardActionListener {
public:
    virtual void OnKeyAction(const KeyEvent& keyEvent) = 0;

public:
    virtual ~IKeyboardActionListener() = default;
};
} // namespace PreVEngine

#endif