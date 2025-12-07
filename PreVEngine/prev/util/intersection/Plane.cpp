#include "Plane.h"

#include "../Utils.h"

namespace prev::util::intersection {
Plane::Plane()
    : normal(glm::vec3{ 0.0f, 1.0f, 0.0f })
    , distance(0.0f)
{
}

Plane::Plane(const glm::vec3& n, const float d)
{
    const float len{ glm::length(n) };
    const float invLen{ 1.0f / len };

    normal = n * invLen;
    distance = d * invLen;
}

Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    const auto v1{ p2 - p1 };
    const auto v2{ p3 - p1 };
    const auto n{ glm::cross(v1, v2) };
    const float len{ glm::length(n) };
    const float invLen{ 1.0f / len };
    if (len < 1e-6f) {
        normal = glm::vec3(0.0f);
        distance = 0.0f;
    } else {
        normal = n * invLen;
        distance = glm::dot(normal, p1);
    }
}

std::ostream& operator<<(std::ostream& out, const Plane& plane)
{
    out << "Normal:    " << prev::util::string::GetAsString(plane.normal, 2) << std::endl;
    out << "Distance: " << plane.distance << std::endl;
    return out;
}
} // namespace prev::util::intersection
