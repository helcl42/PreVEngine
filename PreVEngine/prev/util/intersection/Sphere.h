#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "../../common/Common.h"

#include <iostream>

namespace prev::util::intersection {
struct Sphere {
    glm::vec3 position;

    float radius;

    Sphere();

    Sphere(const glm::vec3& pos, const float rad);

    friend std::ostream& operator<<(std::ostream& out, const Sphere& sphere);
};
} // namespace prev::util::intersection

#endif // !__SPHERE_H__
