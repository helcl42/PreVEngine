#include "Plane.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
Plane::Plane()
    : normal(glm::vec3{ 0.0f, 1.0f, 0.0f })
    , distance(0)
{
}

Plane::Plane(const glm::vec3& n, const float d)
    : normal(glm::normalize(n))
    , distance(d / glm::length(n))
{
}

Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    const auto v1 = p2 - p1;
    const auto v2 = p3 - p1;
    const auto n = glm::cross(v1, v2);
    normal = glm::normalize(n);
    distance = glm::dot(n, p1);
}

std::ostream& operator<<(std::ostream& out, const Plane& plane)
{
    out << "Normal:    " << prev::util::string::GetAsString(plane.normal, 2) << std::endl;
    out << "Distance: " << plane.distance << std::endl;
    return out;
}
} // namespace prev_test::common::intersection
