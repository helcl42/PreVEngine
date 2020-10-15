#include "AABB.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
AABB::AABB()
    : minExtents(glm::vec3(std::numeric_limits<float>::max()))
    , maxExtents(glm::vec3(std::numeric_limits<float>::min()))
{
}

AABB::AABB(const float radius)
    : minExtents(glm::vec3{ -radius })
    , maxExtents(glm::vec3{ radius })
{
}

AABB::AABB(const glm::vec3& minExtents, const glm::vec3& maxExtents)
    : minExtents(minExtents)
    , maxExtents(maxExtents)
{
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
    out << "MinExtents:    " << prev::util::StringUtils::GetAsString(aabb.minExtents, 2) << std::endl;
    out << "MaxExtents:    " << prev::util::StringUtils::GetAsString(aabb.maxExtents, 2) << std::endl;
    return out;
}
} // namespace prev_test::common::intersection
