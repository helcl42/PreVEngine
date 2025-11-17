#include "OBB.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
OBB::OBB()
    : orientation()
    , position(-std::numeric_limits<float>::max())
    , halfExtents(-std::numeric_limits<float>::max())
{
}

OBB::OBB(const glm::quat& ori, const glm::vec3& pos, const glm::vec3& hExtents)
    : orientation(ori)
    , position(pos)
    , halfExtents(hExtents)
{
}

glm::vec3 OBB::GetSize() const
{
    return halfExtents * 2.0f;
}

glm::vec3 OBB::GetHalfSize() const
{
    return halfExtents;
}

std::vector<glm::vec3> OBB::GetPoints() const
{
    std::vector<glm::vec3> result(8);

    const glm::mat3 orientationMatrix{ glm::mat3_cast(orientation) };
    const glm::vec3 axis[] = {
        orientationMatrix[0],
        orientationMatrix[1],
        orientationMatrix[2]
    };

    result[0] = position + axis[0] * halfExtents[0] + axis[1] * halfExtents[1] + axis[2] * halfExtents[2];
    result[1] = position - axis[0] * halfExtents[0] + axis[1] * halfExtents[1] + axis[2] * halfExtents[2];
    result[2] = position + axis[0] * halfExtents[0] - axis[1] * halfExtents[1] + axis[2] * halfExtents[2];
    result[3] = position + axis[0] * halfExtents[0] + axis[1] * halfExtents[1] - axis[2] * halfExtents[2];
    result[4] = position - axis[0] * halfExtents[0] - axis[1] * halfExtents[1] - axis[2] * halfExtents[2];
    result[5] = position + axis[0] * halfExtents[0] - axis[1] * halfExtents[1] - axis[2] * halfExtents[2];
    result[6] = position - axis[0] * halfExtents[0] + axis[1] * halfExtents[1] - axis[2] * halfExtents[2];
    result[7] = position - axis[0] * halfExtents[0] - axis[1] * halfExtents[1] + axis[2] * halfExtents[2];

    return result;
}

std::ostream& operator<<(std::ostream& out, const OBB& aabb)
{
    out << "Orientation: " << prev::util::string::GetAsString(aabb.orientation, 2) << std::endl;
    out << "Position:    " << prev::util::string::GetAsString(aabb.position, 2) << std::endl;
    out << "HalfExtents: " << prev::util::string::GetAsString(aabb.halfExtents, 2) << std::endl;
    return out;
}
} // namespace prev_test::common::intersection