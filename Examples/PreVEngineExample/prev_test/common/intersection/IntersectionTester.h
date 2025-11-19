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

namespace prev_test::common::intersection::tester {
bool Intersects(const Sphere& sphere, const Plane& plane);

bool Intersects(const AABB& box, const Plane& plane);

bool Intersects(const Plane& plane, const Point& point);

bool Intersects(const AABB& box, const Point& point);

bool Intersects(const Sphere& sphere, const Point& point);

bool Intersects(const AABB& box1, const AABB& box2);

bool Intersects(const Sphere& sphere1, const Sphere& sphere2);

bool Intersects(const Sphere& sphere, const AABB& box);

bool Intersects(const Frustum& frustum, const Point& point);

bool Intersects(const Frustum& frustum, const Sphere& sphere);

bool Intersects(const Frustum& frustum, const AABB& box);

bool Intersects(const OBB& obb1, const OBB& obb2);

bool Intersects(const OBB& obb, const AABB& box);

bool Intersects(const OBB& obb, const Sphere& sphere);

bool Intersects(const OBB& obb, const Point& point);

bool Intersects(const OBB& obb, const Frustum& frustum);

bool Intersects(const OBB& obb, const Plane& plane);

bool Intersects(const Ray& ray, const AABB& box, RayCastResult& result);

bool Intersects(const Ray& ray, const Sphere& sphere, RayCastResult& result);

bool Intersects(const Ray& ray, const Plane& plane, RayCastResult& result);

bool Intersects(const Ray& ray, const OBB& obb, RayCastResult& result);
} // namespace prev_test::common::intersection::tester

#endif // !__INTERSECTION_UTIL_H__
