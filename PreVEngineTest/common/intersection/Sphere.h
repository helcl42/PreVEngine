#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <prev/common/Common.h>

namespace prev_test::common::intersection {
struct Sphere {
    glm::vec3 position;

    float radius;

    Sphere();

    Sphere(const glm::vec3& pos, const float rad);

    friend std::ostream& operator<<(std::ostream& out, const Sphere& sphere);
};
} // namespace prev_test::common::intersection

#endif // !__SPHERE_H__
