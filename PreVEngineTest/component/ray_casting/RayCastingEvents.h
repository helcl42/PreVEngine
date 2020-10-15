#ifndef __RAYCASTING_EVENTS_H__
#define __RAYCASTING_EVENTS_H__

#include "../../common/intersection/Ray.h"

namespace prev_test::component::ray_casting {
struct RayEvent {
    prev_test::common::intersection::Ray ray;

    RayEvent() = default;

    RayEvent(const prev_test::common::intersection::Ray& r)
        : ray(r)
    {
    }

    ~RayEvent() = default;
};
} // namespace prev_test::component::ray_casting

#endif // !__RAYCASTING_EVENTS_H__
