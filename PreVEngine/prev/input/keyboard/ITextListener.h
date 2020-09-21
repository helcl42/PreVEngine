#ifndef __ITEXT_LISTENER_H__
#define __ITEXT_LISTENER_H__

#include "KeyboardEvents.h"

namespace prev::input::keyboard {
class ITextListener {
public:
    virtual void OnText(const TextEvent& textEvent) = 0;

public:
    virtual ~ITextListener() = default;
};
} // namespace prev::input::keyboard

#endif