#ifndef __AABB_H__
#define __AABB_H__

#include <prev/common/Common.h>

#include <iostream>
#include <vector>

namespace prev_test::common::intersection {
struct AABB {
    glm::vec3 minExtents;

    glm::vec3 maxExtents;

    AABB();

    AABB(const float radius);

    AABB(const glm::vec3& minExtents, const glm::vec3& maxExtents);

    glm::vec3 GetCenter() const;

    glm::vec3 GetSize() const;

    glm::vec3 GetHalfSize() const;

    std::vector<glm::vec3> GetPoints() const;

    friend std::ostream& operator<<(std::ostream& out, const AABB& aabb);
};
} // namespace prev_test::common::intersection

#endif // !__AABB_H__
