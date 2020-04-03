#ifndef __CULLING_H__
#define __CULLING_H__

#include <Common.h>

#define CMP(x, y) \
    (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

struct Point {
    glm::vec3 position;

    Point()
        : position(glm::vec3(0))
    {
    }

    Point(const glm::vec3& pos)
        : position(pos)
    {
    }
};

struct Ray {
    glm::vec3 startPosition;

    glm::vec3 direction;

    float length;

    Ray()
        : startPosition(glm::vec3(0))
        , direction(glm::vec3(0))
        , length(0)
    {
    }

    Ray(const glm::vec3& pos, const glm::vec3& dir, const float len)
        : startPosition(pos)
        , direction(glm::normalize(dir))
        , length(len)
    {
    }
};

struct Plane {
    glm::vec3 normal;

    float distance;

    Plane()
        : normal(glm::vec3(0, 1, 0))
        , distance(0)
    {
    }

    Plane(const glm::vec3& n, const float d)
        : normal(glm::normalize(n))
        , distance(d * glm::length(n))
    {
    }

    Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
    {
        const auto v1 = p2 - p1;
        const auto v2 = p3 - p1;
        const auto n = glm::cross(v1, v2);
        normal = glm::normalize(n);
        distance = glm::dot(n, p1);
    }
};

struct Sphere {
    glm::vec3 position;

    float radius;

    Sphere()
        : position(glm::vec3(0))
        , radius(0)
    {
    }

    Sphere(const glm::vec3& pos, const float rad)
        : position(pos)
        , radius(rad)
    {
    }
};

struct AABB {
    glm::vec3 minExtents;

    glm::vec3 maxExtents;

    AABB(const float radius)
        : minExtents(glm::vec3(-radius))
        , maxExtents(glm::vec3(radius))
    {
    }

    AABB(const glm::vec3& minExtents, const glm::vec3& maxExtents)
        : minExtents(minExtents)
        , maxExtents(maxExtents)
    {
    }

    glm::vec3 GetCenter() const
    {
        return (minExtents + maxExtents) / 2.0f;
    }

    glm::vec3 GetSize() const
    {
        return maxExtents - minExtents;
    }
};

struct Frustum {
    enum Side {
        LEFT = 0,
        RIGHT = 1,
        TOP = 2,
        BOTTOM = 3,
        FRONT = 4, // near
        BACK = 5 // far
    };

    std::array<Plane, 6> planes;

    std::array<Point, 8> points;

    Frustum() = default;

    Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
    {
        const auto viewProjectionMatrix = projectionMatrix * viewMatrix;
        const auto inverseViewProjectionMatrix = glm::inverse(viewProjectionMatrix);

        const glm::vec3 col1(viewProjectionMatrix[0].x, viewProjectionMatrix[1].x, viewProjectionMatrix[2].x);
        const glm::vec3 col2(viewProjectionMatrix[0].y, viewProjectionMatrix[1].y, viewProjectionMatrix[2].y);
        const glm::vec3 col3(viewProjectionMatrix[0].z, viewProjectionMatrix[1].z, viewProjectionMatrix[2].z);
        const glm::vec3 col4(viewProjectionMatrix[0].w, viewProjectionMatrix[1].w, viewProjectionMatrix[2].w);

        planes[Side::LEFT] = Plane(col4 + col1, viewProjectionMatrix[3].w + viewProjectionMatrix[3].x),
        planes[Side::RIGHT] = Plane(col4 - col1, viewProjectionMatrix[3].w - viewProjectionMatrix[3].x);
        planes[Side::BOTTOM] = Plane(col4 + col2, viewProjectionMatrix[3].w + viewProjectionMatrix[3].y);
        planes[Side::TOP] = Plane(col4 - col2, viewProjectionMatrix[3].w - viewProjectionMatrix[3].y);
        planes[Side::FRONT] = Plane(col4 + col3, viewProjectionMatrix[3].w + viewProjectionMatrix[3].z);
        planes[Side::BACK] = Plane(col4 - col3, viewProjectionMatrix[3].w - viewProjectionMatrix[3].z);

        auto frustumCorners = MathUtil::GetFrustumCorners(inverseViewProjectionMatrix);
        for (auto i = 0; i < frustumCorners.size(); i++) {
            points[i] = Point(frustumCorners[i]);
        }
    }
};

class Culling {
public:
    static bool Intersects(const Sphere& sphere, const Plane& plane) {
        if ((plane.normal.x * sphere.position.x) + (plane.normal.y * sphere.position.y) + (plane.normal.z * sphere.position.z) + plane.distance <= -sphere.radius) {
            return false;
        }
        return true;
    }

    static bool Intersects(const AABB& box, const Plane& plane)
    {
        const glm::vec3 points[] = {
            { box.minExtents.x, box.minExtents.y, box.minExtents.z },
            { box.maxExtents.x, box.minExtents.y, box.minExtents.z },
            { box.minExtents.x, box.maxExtents.y, box.minExtents.z },
            { box.maxExtents.x, box.maxExtents.y, box.minExtents.z },
            { box.minExtents.x, box.minExtents.y, box.maxExtents.z },
            { box.maxExtents.x, box.minExtents.y, box.maxExtents.z },
            { box.minExtents.x, box.maxExtents.y, box.maxExtents.z },
            { box.maxExtents.x, box.maxExtents.y, box.maxExtents.z },
        };
       
        for (const auto point : points) {
            if (glm::dot(glm::vec4(plane.normal, plane.distance), glm::vec4(point, 1.0f)) > 0.0f) {
                return false;
            }
        }
        return true;
    }

    static bool Intersects(const Plane& plane, const Point& point)
    {
        return CMP(glm::dot(point.position, plane.normal) - plane.distance, 0.0f);
    }

    static bool Intersects(const AABB& box, const Point& point) {
        for (auto i = 0; i < glm::length(point.position); i++) {
            if (point.position[i] > box.maxExtents[i]) {
                return false;
            }

            if (point.position[i] < box.minExtents[i]) {
                return false;
            }
        }        
        return true;
    }

    static bool Intersects(const Sphere& sphere, const Point& point)
    {
        return glm::distance(sphere.position, point.position) < sphere.radius;
    }

    static bool Intersects(const AABB& box1, const AABB& box2)
    {
        return (box1.minExtents.x <= box2.maxExtents.x && box1.maxExtents.x >= box2.minExtents.x) 
            && (box1.minExtents.y <= box2.maxExtents.y && box1.maxExtents.y >= box2.minExtents.y) 
            && (box1.minExtents.z <= box2.maxExtents.z && box1.maxExtents.z >= box2.minExtents.z);
    }

    static bool Intersects(const Sphere& sphere1, const Sphere& sphere2)
    {
        return glm::distance(sphere1.position, sphere2.position) < (sphere1.radius + sphere2.radius);
    }

    static bool Intersects(const Sphere& sphere, const AABB& box)
    {
        // get box closest point to sphere center by clamping
        const float x = std::max(box.minExtents.x, std::min(sphere.position.x, box.maxExtents.x));
        const float y = std::max(box.minExtents.y, std::min(sphere.position.y, box.maxExtents.y));
        const float z = std::max(box.minExtents.z, std::min(sphere.position.z, box.maxExtents.z));
        
        return Intersects(sphere, Point{ glm::vec3{ x, y, z } });
    }

    static bool Intersects(const Frustum& frustum, const Point& point)
    {
        for (const auto& plane : frustum.planes) {
            if (glm::dot(point.position, plane.normal) + plane.distance < 0.0f) {
                return false;
            }
        }
        return true;
    }

    static bool Intersects(const Frustum& frustum, const Sphere& sphere)
    {
        for (size_t i = 0; i < frustum.planes.size(); i++) {
            if (!Intersects(sphere, frustum.planes[i])) {
                return false;
            }
        }
        return true;
    }

    static bool Intersects(const Frustum& frustum, const AABB& box)
    { 
        // check box outside/inside of frustum
        for (uint32_t i = 0; i < frustum.planes.size(); i++) {
            
            if (!Intersects(box, frustum.planes[i])) {
                return false;
            }
        }

        // check frustum corners outside/inside box
        for (uint32_t i = 0; i < frustum.points.size(); i++) {
            if (!Intersects(box, frustum.points[i])) {
                return false;
            }
        }
        return true;
    }

    static bool Intersects(const Ray& ray, const AABB& box)
    {
        // TODO
    }

    static bool Intersects(const Ray& ray, const Sphere& sphere)
    {
        // TODO
    }

    static bool Intersects(const Ray& ray, const Plane& plane)
    {
        // TODO
    }
};

enum class BoundingVolumeType {
    SPHERE = 0,
    AABB
};

class IBoundingVolumeComponent {
public:
    virtual bool IsInFrustum(const Frustum& frustum) = 0;
    
    virtual void Update(const glm::mat4& transform) = 0;
    
public:
    virtual ~IBoundingVolumeComponent() = default;
};

class BoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    bool IsInFrustum(const Frustum& frustum) override
    {
    }

    void Update(const glm::mat4& transform) override
    {
    }
};

class BoundingVolumeComponentFactory {
    std::unique_ptr<IBoundingVolumeComponent> Create(const BoundingVolumeType type, const std::shared_ptr<IMesh>& mesh)
    {
        auto boundingVolumeComponent = std::make_unique<BoundingVolumeComponent>();
        
        return boundingVolumeComponent;
    }
};

#endif // __CULLING_H__
