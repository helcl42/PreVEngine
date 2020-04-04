#ifndef __CULLING_H__
#define __CULLING_H__

#include <Common.h>

//#define RENDER_BOUNDING_VOLUMES

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
        , distance(d / glm::length(n))
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

    AABB()
        : minExtents(glm::vec3(std::numeric_limits<float>::max()))
        , maxExtents(glm::vec3(std::numeric_limits<float>::min()))
    {
    }

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

    glm::vec3 GetHalfSize() const
    {
        return GetSize() * 0.5f;
    }

    std::vector<glm::vec3> GetPoints() const
    {
        return {
            { minExtents.x, minExtents.y, minExtents.z },
            { minExtents.x, maxExtents.y, minExtents.z },
            { minExtents.x, minExtents.y, maxExtents.z },
            { minExtents.x, maxExtents.y, maxExtents.z },
            { maxExtents.x, minExtents.y, minExtents.z },
            { maxExtents.x, maxExtents.y, minExtents.z },
            { maxExtents.x, minExtents.y, maxExtents.z },
            { maxExtents.x, maxExtents.y, maxExtents.z }
        };
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
        const auto aabbPoints = box.GetPoints();       

        uint32_t missCount{ 0 };
        for (const auto aabbPoint : aabbPoints) {
            if (glm::dot(glm::vec4(plane.normal, plane.distance), glm::vec4(aabbPoint, 1.0f)) < 0.0f) {
                missCount++;
            }
        }
        return missCount != aabbPoints.size();
    }

    static bool Intersects(const Plane& plane, const Point& point)
    {
        return CMP(glm::dot(point.position, plane.normal) - plane.distance, 0.0f);
    }

    static bool Intersects(const AABB& box, const Point& point) {
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
        for (auto i = 0; i < frustum.planes.size(); i++) {
            if (!Intersects(sphere, frustum.planes[i])) {
                return false;
            }
        }
        return true;
    }

    static bool Intersects(const Frustum& frustum, const AABB& box)
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
    
    virtual void Update(const glm::mat4& worldTransform) = 0;

    virtual BoundingVolumeType GetType() const = 0;

#ifdef RENDER_BOUNDING_VOLUMES
    virtual std::shared_ptr<IModel> GetModel() const = 0;
#endif
    
public:
    virtual ~IBoundingVolumeComponent() = default;
};

class AABBBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    AABBBoundingVolumeComponent(const AABB& box)
        : m_original(box)
        , m_working(box)
        , m_originalAABBPoints(box.GetPoints())
        , m_vorkingAABBPoints(box.GetPoints())
    {
    }

    ~AABBBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const Frustum& frustum) override
    {
        return Culling::Intersects(frustum, m_working);
    }

    void Update(const glm::mat4& worldTransform) override
    {
        const auto rotationScaleTransform = MathUtil::ExtractRotation(worldTransform);
        const auto translation = MathUtil::ExtractTranslation(worldTransform);

        for (auto i = 0; i < m_originalAABBPoints.size(); i++) {
            m_vorkingAABBPoints[i] = rotationScaleTransform * glm::vec4(m_originalAABBPoints[i], 1.0f);
        }

        glm::vec3 minBound{ std::numeric_limits<float>::max() };
        glm::vec3 maxBound{ std::numeric_limits<float>::min() };
        for (const auto pt : m_vorkingAABBPoints) {
            for (auto i = 0; i < minBound.length(); i++) {
                minBound[i] = std::min(minBound[i], pt[i]);
                maxBound[i] = std::max(maxBound[i], pt[i]);
            }
        }
        
        m_working = AABB(glm::vec3(translation + minBound), glm::vec3(translation + maxBound));
#ifdef RENDER_BOUNDING_VOLUMES
        m_model = GenerateModel(m_working);
#endif
    }

    BoundingVolumeType GetType() const override
    {
        return BoundingVolumeType::AABB;
    }

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }
#endif
private:
#ifdef RENDER_BOUNDING_VOLUMES
    static std::unique_ptr<IModel> GenerateModel(const AABB& aabb)
    {
        const auto aabbPoints = aabb.GetPoints();

        const std::vector<glm::vec3> vertices = {
            // front
            aabbPoints[2],
            aabbPoints[6],
            aabbPoints[7],
            aabbPoints[3],
            // back
            aabbPoints[0],
            aabbPoints[4],
            aabbPoints[5],
            aabbPoints[1],
            // top
            aabbPoints[3],
            aabbPoints[7],
            aabbPoints[5],
            aabbPoints[1],
            // bottom
            aabbPoints[2],
            aabbPoints[6],
            aabbPoints[4],
            aabbPoints[0],
            // left
            aabbPoints[2],
            aabbPoints[3],
            aabbPoints[1],
            aabbPoints[0],
            // rightt
            aabbPoints[1],
            aabbPoints[7],
            aabbPoints[5],
            aabbPoints[4]
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        auto allocator = AllocatorProvider::Instance().GetAllocator();        
        auto vertexBuffer = std::make_unique<VBO>(*allocator);
        vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
        auto indexBuffer = std::make_unique<IBO>(*allocator);
        indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));
        return std::make_unique<Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
    }
#endif
private:
#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<IModel> m_model;
#endif
    AABB m_original;

    std::vector<glm::vec3> m_originalAABBPoints;

    AABB m_working;

    std::vector<glm::vec3> m_vorkingAABBPoints;
};

class SphereBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    SphereBoundingVolumeComponent(const Sphere& sphere)
        : m_original(sphere)
        , m_working(sphere)
    {
    }

    ~SphereBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const Frustum& frustum) override
    {
        return Culling::Intersects(frustum, m_working);
    }

    void Update(const glm::mat4& worldTransform) override
    {
        m_working.position = worldTransform * glm::vec4(m_original.position, 1.0f);
#ifdef RENDER_BOUNDING_VOLUMES
        m_model = GenerateModel(m_working);
#endif
    }

    BoundingVolumeType GetType() const override
    {
        return BoundingVolumeType::SPHERE;
    }

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }
#endif
private:
#ifdef RENDER_BOUNDING_VOLUMES
    static std::unique_ptr<IModel> GenerateModel(const Sphere& sphere)
    {
        // TODO -> should get rid of in release
        return nullptr;
    }
#endif
private:
#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<IModel> m_model;
#endif
    Sphere m_original;

    Sphere m_working;
};

class BoundingVolumeComponentFactory {
public:
    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const std::vector<glm::vec3>& vertices) const
    {
        AABB aabb;
        for (const auto& v : vertices) {
            for (auto i = 0; i < aabb.minExtents.length(); i++) {
                aabb.minExtents[i] = std::min(aabb.minExtents[i], v[i]);
                aabb.maxExtents[i] = std::max(aabb.maxExtents[i], v[i]);
            }
        }
        
        return std::make_unique<AABBBoundingVolumeComponent>(aabb);        
    }

    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const std::vector<glm::vec3>& vertices) const
    {
        AABB aabb{};
        for (const auto& v : vertices) {
            for (auto i = 0; i < aabb.minExtents.length(); i++) {
                aabb.minExtents[i] = std::min(aabb.minExtents[i], v[i]);
                aabb.maxExtents[i] = std::max(aabb.maxExtents[i], v[i]);
            }
        }

        float radius = std::numeric_limits<float>::min();
        const auto boxHalfExtents = aabb.GetHalfSize();
        for (auto i = 0; i < boxHalfExtents.length(); i++) {
            radius = std::max(radius, boxHalfExtents[i]);
        }
        Sphere sphere{ aabb.GetCenter(), radius };

        return std::make_unique<SphereBoundingVolumeComponent>(sphere);
    }
};

#endif // __CULLING_H__
