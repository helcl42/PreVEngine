#ifndef __ITEXT_LISTENER_H__
#define __ITEXT_LISTENER_H__

#include "KeyboardEvents.h"

namespace PreVEngine {
class ITextListener {
public:
    virtual void OnText(const TextEvent& textEvent) = 0;

public:
    virtual ~ITextListener() = default;
};
} // namespace PreVEngine

#endif