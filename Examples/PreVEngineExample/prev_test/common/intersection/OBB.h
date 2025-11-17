#ifndef __OBB_H__
#define __OBB_H__

#include <prev/common/Common.h>

#include <iostream>
#include <vector>

namespace prev_test::common::intersection {
struct OBB {
    glm::quat orientation;

    glm::vec3 position;

    glm::vec3 halfExtents;

    OBB();

    OBB(const glm::quat& orientation, const glm::vec3& position, const glm::vec3& halfExtents);

    glm::vec3 GetSize() const;

    glm::vec3 GetHalfSize() const;

    std::vector<glm::vec3> GetPoints() const;

    friend std::ostream& operator<<(std::ostream& out, const OBB& aabb);
};
} // namespace prev_test::common::intersection

#endif // !__OBB_H__
