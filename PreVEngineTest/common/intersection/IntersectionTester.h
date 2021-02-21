#ifndef __INTERSECTOR_H__
#define __INTERSECTOR_H__

#include "AABB.h"
#include "Frustum.h"
#include "OBB.h"
#include "Plane.h"
#include "Point.h"
#include "Ray.h"
#include "RayCastResult.h"
#include "Sphere.h"

namespace prev_test::common::intersection {
class IntersectionTester {
public:
    static bool Intersects(const Sphere& sphere, const Plane& plane);

    static bool Intersects(const AABB& box, const Plane& plane);

    static bool Intersects(const Plane& plane, const Point& point);

    static bool Intersects(const AABB& box, const Point& point);

    static bool Intersects(const Sphere& sphere, const Point& point);

    static bool Intersects(const AABB& box1, const AABB& box2);

    static bool Intersects(const Sphere& sphere1, const Sphere& sphere2);

    static bool Intersects(const Sphere& sphere, const AABB& box);

    static bool Intersects(const Frustum& frustum, const Point& point);

    static bool Intersects(const Frustum& frustum, const Sphere& sphere);

    static bool Intersects(const Frustum& frustum, const AABB& box);

    static bool Intersects(const OBB& obb1, const OBB& obb2);

    static bool Intersects(const OBB& obb, const AABB& box);

    static bool Intersects(const OBB& obb, const Sphere& sphere);

    static bool Intersects(const OBB& obb, const Point& point);

    static bool Intersects(const OBB& obb, const Frustum& frustum);

    static bool Intersects(const OBB& obb, const Plane& plane);

    static bool Intersects(const Ray& ray, const AABB& box, RayCastResult& result);

    static bool Intersects(const Ray& ray, const Sphere& sphere, RayCastResult& result);

    static bool Intersects(const Ray& ray, const Plane& plane, RayCastResult& result);

    static bool Intersects(const Ray& ray, const OBB& obb, RayCastResult& result);
};
} // namespace prev_test::common::intersection

#endif // !__INTERSECTION_UTIL_H__
