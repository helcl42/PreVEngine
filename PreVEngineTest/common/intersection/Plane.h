#ifndef __PLANE_H__
#define __PLANE_H__

#include <prev/common/Common.h>

namespace prev_test::common::intersection {
struct Plane {
    glm::vec3 normal;

    float distance;

    Plane();

    Plane(const glm::vec3& n, const float d);

    Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

    friend std::ostream& operator<<(std::ostream& out, const Plane& plane);
};
} // namespace prev_test::common::intersection

#endif // !__PLANE_H__
