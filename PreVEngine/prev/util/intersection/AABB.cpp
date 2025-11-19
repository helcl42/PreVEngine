#include "AABB.h"

#include "../Utils.h"

namespace prev::util::intersection {
AABB::AABB()
    : minExtents(glm::vec3(std::numeric_limits<float>::max()))
    , maxExtents(-glm::vec3(std::numeric_limits<float>::max()))
{
}

AABB::AABB(const float radius, const bool inscribed)
    : minExtents(glm::vec3{ inscribed ? (-radius / std::sqrt(1.0f / 3.0f)) : -radius })
    , maxExtents(glm::vec3{ inscribed ? (radius / std::sqrt(1.0f / 3.0f)) : radius })
{
}

AABB::AABB(const glm::vec3& minExtents, const glm::vec3& maxExtents)
    : minExtents(minExtents)
    , maxExtents(maxExtents)
{
}

AABB::AABB(const std::vector<glm::vec3>& points)
    : AABB()
{
    for (const auto& p : points) {
        minExtents = glm::min(minExtents, p);
        maxExtents = glm::max(maxExtents, p);
    }
}

glm::vec3 AABB::GetCenter() const
{
    return (minExtents + maxExtents) / 2.0f;
}

glm::vec3 AABB::GetSize() const
{
    return maxExtents - minExtents;
}

glm::vec3 AABB::GetHalfSize() const
{
    return GetSize() * 0.5f;
}

std::vector<glm::vec3> AABB::GetPoints() const
{
    return {
        { minExtents.x, minExtents.y, minExtents.z },
        { minExtents.x, maxExtents.y, minExtents.z },
        { minExtents.x, minExtents.y, maxExtents.z },
        { minExtents.x, maxExtents.y, maxExtents.z },
        { maxExtents.x, minExtents.y, minExtents.z },
        { maxExtents.x, maxExtents.y, minExtents.z },
        { maxExtents.x, minExtents.y, maxExtents.z },
        { maxExtents.x, maxExtents.y, maxExtents.z }
    };
}

std::ostream& operator<<(std::ostream& out, const AABB& aabb)
{
    out << "MinExtents:    " << prev::util::string::GetAsString(aabb.minExtents, 2) << std::endl;
    out << "MaxExtents:    " << prev::util::string::GetAsString(aabb.maxExtents, 2) << std::endl;
    return out;
}
} // namespace prev::util::intersection
