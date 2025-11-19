#ifndef __PLANE_H__
#define __PLANE_H__

#include "../../common/Common.h"

#include <iostream>

namespace prev::util::intersection {
struct Plane {
    glm::vec3 normal;

    float distance;

    Plane();

    Plane(const glm::vec3& n, const float d);

    Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

    friend std::ostream& operator<<(std::ostream& out, const Plane& plane);
};
} // namespace prev::util::intersection

#endif // !__PLANE_H__
