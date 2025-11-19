#ifndef __RAY_CASTER_EVENTS_H__
#define __RAY_CASTER_EVENTS_H__

#include <prev/util/intersection/Ray.h>

namespace prev_test::scene::ray_casting {
struct RayEvent {
    prev::util::intersection::Ray ray;

    RayEvent() = default;

    RayEvent(const prev::util::intersection::Ray& r)
        : ray(r)
    {
    }

    ~RayEvent() = default;
};
} // namespace prev_test::scene::ray_casting

#endif // !__RAY_CASTER_EVENTS_H__
