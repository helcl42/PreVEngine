#include "Ray.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
Ray::Ray()
    : origin(glm::vec3(0))
    , direction(glm::vec3(0))
    , length(0)
{
}

Ray::Ray(const glm::vec3& pos, const glm::vec3& dir, const float len)
    : origin(pos)
    , direction(glm::normalize(dir))
    , length(len)
{
}

glm::vec3 Ray::GetStartPoint() const
{
    return origin;
}

glm::vec3 Ray::GetEndPoint() const
{
    return GetPointAtDistances(length);
}

glm::vec3 Ray::GetPointAtDistances(const float distance) const
{
    return origin + direction * distance;
}

std::ostream& operator<<(std::ostream& out, const Ray& ray)
{
    out << "Direction: " << prev::util::StringUtils::GetAsString(ray.direction, 2) << std::endl;
    out << "Origin:    " << prev::util::StringUtils::GetAsString(ray.origin, 2) << std::endl;
    out << "Length:    " << ray.length << std::endl;
    return out;
}
} // namespace prev_test::common::intersection
