#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "Plane.h"
#include "Point.h"

#include <array>
#include <iostream>
#include <vector>

namespace prev_test::common::intersection {
struct Frustum {
    std::array<Plane, 6> planes;

    std::array<Point, 8> points;

    Frustum() = default;

    Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

    float GetRadius() const;

    Point GetCenter() const;

    std::vector<glm::vec3> GetPoints() const;

    friend std::ostream& operator<<(std::ostream& out, const Frustum& frustum);
};
} // namespace prev_test::common::intersection

#endif // !__FRUSTUM_H__
