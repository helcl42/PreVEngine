#include "RayCastResult.h"

#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
RayCastResult::RayCastResult()
    : point(glm::vec3{ 0.0f })
    , normal(glm::vec3{ 0.0f, 0.0f, 1.0f })
    , t(-1.0f)
    , hit(false)
{
}

RayCastResult::RayCastResult(const glm::vec3& p, const glm::vec3& n, const float distanceOnRay, const bool wasHit)
    : point(p)
    , normal(n)
    , t(distanceOnRay)
    , hit(wasHit)
{
}

std::ostream& operator<<(std::ostream& out, const RayCastResult& result)
{
    out << "Point: " << prev::util::string::GetAsString(result.point, 2) << std::endl;
    out << "Normal:    " << prev::util::string::GetAsString(result.normal, 2) << std::endl;
    out << "T:    " << result.t << std::endl;
    out << "hit: " << result.hit << std::endl;
    return out;
}
} // namespace prev_test::common::intersection
