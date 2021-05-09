#include "Point.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
Point::Point()
    : position(glm::vec3{ 0.0f })
{
}

Point::Point(const glm::vec3& pos)
    : position(pos)
{
}

std::ostream& operator<<(std::ostream& out, const Point& point)
{
    out << "Position: " << prev::util::string::GetAsString(point.position, 2) << std::endl;
    return out;
}
} // namespace prev_test::common::intersection
