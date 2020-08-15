#ifndef __CORE_EVENTS_H__
#define __CORE_EVENTS_H__

#include <inttypes.h>

namespace prev {
struct NewIterationEvent {
    float deltaTime;

    uint32_t windowWidth;

    uint32_t windowHeight;
};
} // namespace prev

#endif // !__CORE_EVENTS_H__
