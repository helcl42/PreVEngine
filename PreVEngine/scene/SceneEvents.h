#ifndef __SCENE_EVENTS_H__
#define __SCENE_EVENTS_H__

#include <inttypes.h>

namespace prev::scene {
struct SceneNodeShutDownEvent {
    uint64_t id;
};
} // namespace prev::scene

#endif