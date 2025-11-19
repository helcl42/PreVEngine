#include "Point.h"

#include "../Utils.h"

namespace prev::util::intersection {
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
} // namespace prev::util::intersection
