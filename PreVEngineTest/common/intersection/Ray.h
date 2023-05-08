#ifndef __RAY_H__
#define __RAY_H__

#include <prev/util/Utils.h>

#include <iostream>

namespace prev_test::common::intersection {
struct Ray {
    glm::vec3 origin;

    glm::vec3 direction;

    float length;

    Ray();

    Ray(const glm::vec3& pos, const glm::vec3& dir, const float len);

    glm::vec3 GetStartPoint() const;

    glm::vec3 GetEndPoint() const;

    glm::vec3 GetPointAtDistances(const float distance) const;

    friend std::ostream& operator<<(std::ostream& out, const Ray& ray);
};
} // namespace prev_test::common::intersection

#endif // !__RAY_H__
