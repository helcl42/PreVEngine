#include "Sphere.h"

#include "../Utils.h"

namespace prev::util::intersection {
Sphere::Sphere()
    : position(glm::vec3{ 0.0f })
    , radius(0)
{
}

Sphere::Sphere(const glm::vec3& pos, const float rad)
    : position(pos)
    , radius(rad)
{
}

std::ostream& operator<<(std::ostream& out, const Sphere& sphere)
{
    out << "Position:    " << prev::util::string::GetAsString(sphere.position, 2) << std::endl;
    out << "Radius: " << sphere.radius << std::endl;
    return out;
}
} // namespace prev::util::intersection
