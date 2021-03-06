#ifndef __IKEYBOARD_ACTION_LISTENER_H__
#define __IKEYBOARD_ACTION_LISTENER_H__

#include "KeyboardEvents.h"

namespace prev::input::keyboard {
class IKeyboardActionListener {
public:
    virtual void OnKeyAction(const KeyEvent& keyEvent) = 0;

public:
    virtual ~IKeyboardActionListener() = default;
};
} // namespace prev::input::keyboard

#endif