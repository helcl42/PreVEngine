#ifndef __RAYCAST_RESULT_H__
#define __RAYCAST_RESULT_H__

#include "../../common/Common.h"

#include <iostream>

namespace prev::util::intersection {
struct RayCastResult {
    glm::vec3 point;

    glm::vec3 normal;

    float t;

    bool hit;

    explicit RayCastResult();

    explicit RayCastResult(const glm::vec3& p, const glm::vec3& n, const float distanceOnRay, const bool wasHit);

    friend std::ostream& operator<<(std::ostream& out, const RayCastResult& result);
};
} // namespace prev::util::intersection

#endif // !__RAYCAST_RESULT_H__
