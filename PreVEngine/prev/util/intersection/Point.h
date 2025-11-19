#ifndef __POINT_H__
#define __POINT_H__

#include "../../common/Common.h"

#include <iostream>

namespace prev::util::intersection {
struct Point {
    glm::vec3 position;

    Point();

    Point(const glm::vec3& pos);

    friend std::ostream& operator<<(std::ostream& out, const Point& point);
};
} // namespace prev::util::intersection

#endif // !__POINT_H__
