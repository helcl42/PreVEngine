#ifndef __CORE_EVENTS_H__
#define __CORE_EVENTS_H__

#include <inttypes.h>

namespace prev::core {
struct NewIterationEvent {
    float deltaTime{};

    uint32_t windowWidth{};

    uint32_t windowHeight{};
};
} // namespace prev::core

#endif // !__CORE_EVENTS_H__
