#ifndef __CORE_EVENTS_H__
#define __CORE_EVENTS_H__

#include <inttypes.h>

namespace PreVEngine {
struct NewIterationEvent {
    float deltaTime;

    uint32_t windowWidth;

    uint32_t windowHeight;
};
} // namespace PreVEngine

#endif // !__CORE_EVENTS_H__
