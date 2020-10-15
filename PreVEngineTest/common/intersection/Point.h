#ifndef __POINT_H__
#define __POINT_H__

#include <prev/common/Common.h>

namespace prev_test::common::intersection {
struct Point {
    glm::vec3 position;

    Point();

    Point(const glm::vec3& pos);

    friend std::ostream& operator<<(std::ostream& out, const Point& point);
};
} // namespace prev_test::common::intersection

#endif // !__POINT_H__
