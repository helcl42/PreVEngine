#include "IntersectionTester.h"

#define CMP(x, y) \
    (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

namespace prev_test::common::intersection {
bool IntersectionTester::Intersects(const Sphere& sphere, const Plane& plane)
{
    if ((plane.normal.x * sphere.position.x) + (plane.normal.y * sphere.position.y) + (plane.normal.z * sphere.position.z) + plane.distance <= -sphere.radius) {
        return false;
    }
    return true;
}

bool IntersectionTester::Intersects(const AABB& box, const Plane& plane)
{
    const auto aabbPoints = box.GetPoints();

    // TODO - optimize ?
    uint32_t missCount{ 0 };
    for (const auto aabbPoint : aabbPoints) {
        if (glm::dot(glm::vec4(plane.normal, plane.distance), glm::vec4(aabbPoint, 1.0f)) < 0.0f) {
            missCount++;
        }
    }
    return missCount != aabbPoints.size();
}

bool IntersectionTester::Intersects(const Plane& plane, const Point& point)
{
    return CMP(glm::dot(point.position, plane.normal) - plane.distance, 0.0f);
}

bool IntersectionTester::Intersects(const AABB& box, const Point& point)
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

bool IntersectionTester::Intersects(const Sphere& sphere, const Point& point)
{
    return glm::distance(sphere.position, point.position) < sphere.radius;
}

bool IntersectionTester::Intersects(const AABB& box1, const AABB& box2)
{
    return (box1.minExtents.x <= box2.maxExtents.x && box1.maxExtents.x >= box2.minExtents.x)
        && (box1.minExtents.y <= box2.maxExtents.y && box1.maxExtents.y >= box2.minExtents.y)
        && (box1.minExtents.z <= box2.maxExtents.z && box1.maxExtents.z >= box2.minExtents.z);
}

bool IntersectionTester::Intersects(const Sphere& sphere1, const Sphere& sphere2)
{
    return glm::distance(sphere1.position, sphere2.position) < (sphere1.radius + sphere2.radius);
}

bool IntersectionTester::Intersects(const Sphere& sphere, const AABB& box)
{
    // get box closest point to sphere center by clamping
    const float x = std::max(box.minExtents.x, std::min(sphere.position.x, box.maxExtents.x));
    const float y = std::max(box.minExtents.y, std::min(sphere.position.y, box.maxExtents.y));
    const float z = std::max(box.minExtents.z, std::min(sphere.position.z, box.maxExtents.z));

    return Intersects(sphere, Point{ glm::vec3{ x, y, z } });
}

bool IntersectionTester::Intersects(const Frustum& frustum, const Point& point)
{
    for (const auto& plane : frustum.planes) {
        if (glm::dot(point.position, plane.normal) + plane.distance < 0.0f) {
            return false;
        }
    }
    return true;
}

bool IntersectionTester::Intersects(const Frustum& frustum, const Sphere& sphere)
{
    for (auto i = 0; i < frustum.planes.size(); i++) {
        if (!Intersects(sphere, frustum.planes[i])) {
            return false;
        }
    }
    return true;
}

bool IntersectionTester::Intersects(const Frustum& frustum, const AABB& box)
{
    // check box outside/inside of frustum
    for (const auto plane : frustum.planes) {
        if (!Intersects(box, plane)) {
            return false;
        }
    }

    // check frustum corners outside/inside box
    for (auto i = 0; i < box.maxExtents.length(); i++) {
        uint32_t out = 0;
        for (const auto point : frustum.points) {
            if (point.position[i] > box.maxExtents[i]) {
                out++;
            }
        }
        if (out == frustum.points.size()) {
            return false;
        }
        out = 0;
        for (const auto point : frustum.points) {
            if (point.position[i] < box.minExtents[i]) {
                out++;
            }
        }
        if (out == frustum.points.size()) {
            return false;
        }
    }
    return true;
}

bool IntersectionTester::Intersects(const Ray& ray, const AABB& box, RayCastResult& result)
{
    const glm::vec3 min = box.minExtents;
    const glm::vec3 max = box.maxExtents;

    const float SMALL_FLOAT = 0.00001f;
    float t1 = (min.x - ray.origin.x) / (CMP(ray.direction.x, 0.0f) ? SMALL_FLOAT : ray.direction.x);
    float t2 = (max.x - ray.origin.x) / (CMP(ray.direction.x, 0.0f) ? SMALL_FLOAT : ray.direction.x);
    float t3 = (min.y - ray.origin.y) / (CMP(ray.direction.y, 0.0f) ? SMALL_FLOAT : ray.direction.y);
    float t4 = (max.y - ray.origin.y) / (CMP(ray.direction.y, 0.0f) ? SMALL_FLOAT : ray.direction.y);
    float t5 = (min.z - ray.origin.z) / (CMP(ray.direction.z, 0.0f) ? SMALL_FLOAT : ray.direction.z);
    float t6 = (max.z - ray.origin.z) / (CMP(ray.direction.z, 0.0f) ? SMALL_FLOAT : ray.direction.z);

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
    float t[] = { t1, t2, t3, t4, t5, t6 };
    for (auto i = 0; i < 6; ++i) {
        if (CMP(tResult, t[i])) {
            resultNormal = normals[i];
        }
    }

    result.t = tResult;
    result.hit = true;
    result.point = ray.origin + ray.direction * tResult;
    result.normal = resultNormal;

    return true;
}

bool IntersectionTester::Intersects(const Ray& ray, const Sphere& sphere, RayCastResult& result)
{
    const glm::vec3 e = sphere.position - ray.origin;
    const float radiusSquared = sphere.radius * sphere.radius;

    float eSq = glm::dot(e, e); // squared distance
    float a = glm::dot(e, ray.direction);

    if (radiusSquared - (eSq - a * a) < 0.0f) {
        return false;
    }

    float bSq = eSq - (a * a);
    float f = sqrt(fabsf(radiusSquared - bSq));

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

bool IntersectionTester::Intersects(const Ray& ray, const Plane& plane, RayCastResult& result)
{
    const float EPSILON = 0.0001f;
    float denom = glm::dot(ray.direction, plane.normal);
    if (fabsf(denom) > EPSILON) {
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
} // namespace prev_test::common::intersection
