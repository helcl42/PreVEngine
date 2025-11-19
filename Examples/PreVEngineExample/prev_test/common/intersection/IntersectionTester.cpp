#include "IntersectionTester.h"

#include <prev/util/MathUtils.h>

namespace prev_test::common::intersection::tester {
namespace {
    struct Interval {
        float min;
        float max;
    };

    static Interval GetInterval(const std::vector<glm::vec3>& points, const glm::vec3& axis)
    {
        Interval result{};
        result.min = result.max = glm::dot(axis, points[0]);
        for (int i = 1; i < 8; i++) {
            const float projection{ glm::dot(axis, points[i]) };
            result.min = (projection < result.min) ? projection : result.min;
            result.max = (projection > result.max) ? projection : result.max;
        }
        return result;
    }

    static Interval GetInterval(const OBB& obb, const glm::vec3& axis)
    {
        const auto obbPoints{ obb.GetPoints() };
        return GetInterval(obbPoints, axis);
    }

    static Interval GetInterval(const AABB& aabb, const glm::vec3& axis)
    {
        const auto aabbPoints{ aabb.GetPoints() };
        return GetInterval(aabbPoints, axis);
    }

    static bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis)
    {
        const Interval a{ GetInterval(aabb, axis) };
        const Interval b{ GetInterval(obb, axis) };
        return (b.min <= a.max) && (a.min <= b.max);
    }

    static bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis)
    {
        const Interval a{ GetInterval(obb1, axis) };
        const Interval b{ GetInterval(obb2, axis) };
        return (b.min <= a.max) && (a.min <= b.max);
    }

    static glm::vec3 GetClosestPoint(const OBB& obb, const glm::vec3& point)
    {
        const glm::vec3 dir{ point - obb.position };
        const glm::mat3 orientationMatrix{ glm::mat3_cast(obb.orientation) };

        glm::vec3 result{ obb.position };
        for (int i = 0; i < 3; i++) {
            const glm::vec3 axis{ orientationMatrix[i] };
            float distance{ glm::dot(dir, axis) };
            if (distance > obb.halfExtents[i]) {
                distance = obb.halfExtents[i];
            }
            if (distance < -obb.halfExtents[i]) {
                distance = -obb.halfExtents[i];
            }
            result = result + (axis * distance);
        }
        return result;
    }
} // namespace

bool Intersects(const Sphere& sphere, const Plane& plane)
{
    if ((plane.normal.x * sphere.position.x) + (plane.normal.y * sphere.position.y) + (plane.normal.z * sphere.position.z) + plane.distance <= -sphere.radius) {
        return false;
    }
    return true;
}

bool Intersects(const AABB& box, const Plane& plane)
{
    const auto aabbPoints{ box.GetPoints() };

    uint32_t missCount{ 0 };
    for (const auto& aabbPoint : aabbPoints) {
        if (glm::dot(glm::vec4(plane.normal, plane.distance), glm::vec4(aabbPoint, 1.0f)) < 0.0f) {
            missCount++;
        }
    }
    return missCount != aabbPoints.size();
}

bool Intersects(const Plane& plane, const Point& point)
{
    return prev::util::math::AlmostZero(glm::dot(point.position, plane.normal) - plane.distance);
}

bool Intersects(const AABB& box, const Point& point)
{
    for (auto i = 0; i < point.position.length(); i++) {
        if (point.position[i] > box.maxExtents[i]) {
            return false;
        }

        if (point.position[i] < box.minExtents[i]) {
            return false;
        }
    }
    return true;
}

bool Intersects(const Sphere& sphere, const Point& point)
{
    return glm::distance(sphere.position, point.position) < sphere.radius;
}

bool Intersects(const AABB& box1, const AABB& box2)
{
    return (box1.minExtents.x <= box2.maxExtents.x && box1.maxExtents.x >= box2.minExtents.x)
        && (box1.minExtents.y <= box2.maxExtents.y && box1.maxExtents.y >= box2.minExtents.y)
        && (box1.minExtents.z <= box2.maxExtents.z && box1.maxExtents.z >= box2.minExtents.z);
}

bool Intersects(const Sphere& sphere1, const Sphere& sphere2)
{
    return glm::distance(sphere1.position, sphere2.position) < (sphere1.radius + sphere2.radius);
}

bool Intersects(const Sphere& sphere, const AABB& box)
{
    // get box closest point to sphere center by clamping
    const float x{ std::max(box.minExtents.x, std::min(sphere.position.x, box.maxExtents.x)) };
    const float y{ std::max(box.minExtents.y, std::min(sphere.position.y, box.maxExtents.y)) };
    const float z{ std::max(box.minExtents.z, std::min(sphere.position.z, box.maxExtents.z)) };

    return Intersects(sphere, Point{ glm::vec3{ x, y, z } });
}

bool Intersects(const Frustum& frustum, const Point& point)
{
    for (const auto& plane : frustum.planes) {
        if (glm::dot(point.position, plane.normal) + plane.distance < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Intersects(const Frustum& frustum, const Sphere& sphere)
{
    for (size_t i = 0; i < frustum.planes.size(); i++) {
        if (!Intersects(sphere, frustum.planes[i])) {
            return false;
        }
    }
    return true;
}

bool Intersects(const Frustum& frustum, const AABB& box)
{
    // check box outside/inside of frustum
    for (const auto& plane : frustum.planes) {
        if (!Intersects(box, plane)) {
            return false;
        }
    }

    uint32_t missCount{ 9 };
    for (const auto& point : frustum.points) {
        if (!Intersects(box, point.position)) {
            missCount++;
        }
    }

    if (missCount == frustum.points.size()) {
        return false;
    }

    return true;
}

bool Intersects(const OBB& obb1, const OBB& obb2)
{
    const glm::mat3 obbAOrientationMatrix{ glm::mat3_cast(obb1.orientation) };
    const glm::mat3 obbBOrientationMatrix{ glm::mat3_cast(obb2.orientation) };

    glm::vec3 testAxis[15] = {
        obbAOrientationMatrix[0],
        obbAOrientationMatrix[1],
        obbAOrientationMatrix[2],
        obbBOrientationMatrix[0],
        obbBOrientationMatrix[1],
        obbBOrientationMatrix[2]
    };
    for (int i = 0; i < 3; i++) // Fill out rest of axis
    {
        testAxis[6 + i * 3 + 0] = glm::cross(testAxis[i], testAxis[0]);
        testAxis[6 + i * 3 + 1] = glm::cross(testAxis[i], testAxis[1]);
        testAxis[6 + i * 3 + 2] = glm::cross(testAxis[i], testAxis[2]);
    }

    for (int i = 0; i < 15; i++) {
        if (!OverlapOnAxis(obb1, obb2, testAxis[i])) {
            return false;
        }
    }
    return true;
}

bool Intersects(const OBB& obb, const AABB& box)
{
    const glm::mat3 obbOrientationMatrix{ glm::mat3_cast(obb.orientation) };

    glm::vec3 test[15] = {
        glm::vec3(1, 0, 0), // AABB axis 1
        glm::vec3(0, 1, 0), // AABB axis 2
        glm::vec3(0, 0, 1), // AABB axis 3
        glm::vec3(obbOrientationMatrix[0]),
        glm::vec3(obbOrientationMatrix[1]),
        glm::vec3(obbOrientationMatrix[2])
    };

    for (int i = 0; i < 3; i++) // Fill out rest of axis
    {
        test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
        test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
        test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    for (int i = 0; i < 15; i++) {
        if (!OverlapOnAxis(box, obb, test[i])) {
            return false;
        }
    }
    return true;
}

bool Intersects(const OBB& obb, const Sphere& sphere)
{
    const glm::vec3 closestPoint{ GetClosestPoint(obb, sphere.position) };
    const float distance{ glm::length(closestPoint - sphere.position) };
    if (distance > sphere.radius) {
        return false;
    }
    return true;
}

bool Intersects(const OBB& obb, const Point& point)
{
    const glm::vec3 dir{ point.position - obb.position };
    const glm::mat3 orientationMatrix{ glm::mat3_cast(obb.orientation) };
    for (int i = 0; i < 3; i++) {
        const glm::vec3 axis{ orientationMatrix[i] };
        const float distance{ glm::dot(dir, axis) };
        if (distance > obb.halfExtents[i]) {
            return false;
        }
        if (distance < -obb.halfExtents[i]) {
            return false;
        }
    }
    return true;
}

bool Intersects(const OBB& obb, const Frustum& frustum)
{
    // check box outside/inside of frustum
    for (const auto& plane : frustum.planes) {
        if (!Intersects(obb, plane)) {
            return false;
        }
    }

    uint32_t missCount{ 9 };
    for (const auto& point : frustum.points) {
        if (!Intersects(obb, point.position)) {
            missCount++;
        }
    }

    if (missCount == frustum.points.size()) {
        return false;
    }

    return true;
}

bool Intersects(const OBB& obb, const Plane& plane)
{
    const auto obbPoints{ obb.GetPoints() };

    uint32_t missCount{ 0 };
    for (const auto& obbPoint : obbPoints) {
        if (glm::dot(glm::vec4(plane.normal, plane.distance), glm::vec4(obbPoint, 1.0f)) < 0.0f) {
            missCount++;
        }
    }
    return missCount != obbPoints.size();
}

bool Intersects(const Ray& ray, const AABB& box, RayCastResult& result)
{
    const glm::vec3 min = box.minExtents;
    const glm::vec3 max = box.maxExtents;

    const float EPSILON{ 0.00001f };
    float t1 = (min.x - ray.origin.x) / (prev::util::math::AlmostZero(ray.direction.x) ? EPSILON : ray.direction.x);
    float t2 = (max.x - ray.origin.x) / (prev::util::math::AlmostZero(ray.direction.x) ? EPSILON : ray.direction.x);
    float t3 = (min.y - ray.origin.y) / (prev::util::math::AlmostZero(ray.direction.y) ? EPSILON : ray.direction.y);
    float t4 = (max.y - ray.origin.y) / (prev::util::math::AlmostZero(ray.direction.y) ? EPSILON : ray.direction.y);
    float t5 = (min.z - ray.origin.z) / (prev::util::math::AlmostZero(ray.direction.z) ? EPSILON : ray.direction.z);
    float t6 = (max.z - ray.origin.z) / (prev::util::math::AlmostZero(ray.direction.z) ? EPSILON : ray.direction.z);

    float tmin = std::fmaxf(std::fmaxf(std::fminf(t1, t2), std::fminf(t3, t4)), std::fminf(t5, t6));
    float tmax = std::fminf(std::fminf(std::fmaxf(t1, t2), std::fmaxf(t3, t4)), std::fmaxf(t5, t6));

    if (tmax < 0) {
        return false; // AABB is behind ray's origin
    }

    if (tmin > tmax) {
        return false; // tmin > tmax => ray doesn't intersect AABB
    }

    float tResult = tmin;

    // if tmin is < 0, tmax is closer
    if (tmin < 0.0f) {
        tResult = tmax;
    }

    const glm::vec3 normals[] = {
        { -1, 0, 0 },
        { 1, 0, 0 },
        { 0, -1, 0 },
        { 0, 1, 0 },
        { 0, 0, -1 },
        { 0, 0, 1 }
    };

    glm::vec3 resultNormal;
    const float t[] = { t1, t2, t3, t4, t5, t6 };
    for (auto i = 0; i < 6; ++i) {
        if (prev::util::math::AlmostEqual(tResult, t[i])) {
            resultNormal = normals[i];
        }
    }

    result.t = tResult;
    result.hit = true;
    result.point = ray.origin + ray.direction * tResult;
    result.normal = resultNormal;

    return true;
}

bool Intersects(const Ray& ray, const Sphere& sphere, RayCastResult& result)
{
    const auto e{ sphere.position - ray.origin };
    const float radiusSquared{ sphere.radius * sphere.radius };

    const float eSq{ glm::dot(e, e) }; // squared distance
    const float a{ glm::dot(e, ray.direction) };

    if (radiusSquared - (eSq - a * a) < 0.0f) {
        return false;
    }

    const float bSq{ eSq - (a * a) };
    const float f{ std::sqrt(std::abs(radiusSquared - bSq)) };

    float t;
    if (eSq < radiusSquared) { // Ray starts inside the sphere
        t = a + f; // reverse direction
    } else {
        t = a - f;
    }

    result.t = t;
    result.hit = true;
    result.point = ray.origin + t * ray.direction;
    result.normal = glm::normalize(result.point - sphere.position);
    return true;
}

bool Intersects(const Ray& ray, const Plane& plane, RayCastResult& result)
{
    const float EPSILON{ 0.0001f };
    const float denom{ glm::dot(ray.direction, plane.normal) };
    if (std::abs(denom) > EPSILON) {
        float pn = glm::dot(ray.origin, plane.normal);
        float t = -(plane.distance + pn) / denom;
        if (t >= EPSILON) {
            result.t = t;
            result.hit = true;
            result.point = ray.origin + t * ray.direction;
            result.normal = plane.normal;
            return true;
        }
    }
    return false;
}

bool Intersects(const Ray& ray, const OBB& obb, RayCastResult& result)
{
    const glm::vec3 size{ obb.halfExtents };
    const glm::vec3 p{ obb.position - ray.origin };

    const glm::mat3 orientationMatrix{ glm::mat3_cast(obb.orientation) };
    const glm::vec3 X(orientationMatrix[0]);
    const glm::vec3 Y(orientationMatrix[1]);
    const glm::vec3 Z(orientationMatrix[2]);

    const float EPSILON{ 0.0001f };

    glm::vec3 f{ glm::dot(X, ray.direction), glm::dot(Y, ray.direction), glm::dot(Z, ray.direction) };
    glm::vec3 e{ glm::dot(X, p), glm::dot(Y, p), glm::dot(Z, p) };

    float tArray[] = { 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 3; i++) {
        if (prev::util::math::AlmostZero(f[i])) {
            if (-e[i] - size[i] > 0 || -e[i] + size[i] < 0) {
                return false;
            }

            f[i] = EPSILON; // Avoid div by 0!
        }
        tArray[i * 2 + 0] = (e[i] + size[i]) / f[i]; // tmin[x, y, z]
        tArray[i * 2 + 1] = (e[i] - size[i]) / f[i]; // tmax[x, y, z]
    }

    const float tmin{ std::fmaxf(std::fmaxf(std::fminf(tArray[0], tArray[1]), std::fminf(tArray[2], tArray[3])), std::fminf(tArray[4], tArray[5])) };
    const float tmax{ std::fminf(std::fminf(std::fmaxf(tArray[0], tArray[1]), std::fmaxf(tArray[2], tArray[3])), std::fmaxf(tArray[4], tArray[5])) };

    if (tmax < 0) {
        return false;
    }
    if (tmin > tmax) {
        return false;
    }

    // If tmin is < 0, tmax is closer
    float t{ tmin };
    if (tmin < 0.0f) {
        t = tmax;
    }

    glm::vec3 normals[] = {
        X, // +x
        X * -1.0f, // -x
        Y, // +y
        Y * -1.0f, // -y
        Z, // +z
        Z * -1.0f // -z
    };

    for (int i = 0; i < 6; ++i) {
        if (prev::util::math::AlmostEqual(t, tArray[i])) {
            result.normal = glm::normalize(normals[i]);
        }
    }
    result.hit = true;
    result.t = t;
    result.point = ray.origin + t * ray.direction;

    return true;
}
} // namespace prev_test::common::intersection::tester
