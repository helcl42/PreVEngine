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
    glm::vec3 origin;

    glm::vec3 direction;

    float length;

    Ray()
        : origin(glm::vec3(0))
        , direction(glm::vec3(0))
        , length(0)
    {
    }

    Ray(const glm::vec3& pos, const glm::vec3& dir, const float len)
        : origin(pos)
        , direction(glm::normalize(dir))
        , length(len)
    {
    }

    glm::vec3 GetStartPoint() const
    {
        return origin;
    }

    glm::vec3 GetEndPoint() const
    {
        return GetPointAtDistances(length);
    }

    glm::vec3 GetPointAtDistances(const float distance) const
    {
        return origin + direction * distance;
    }

    friend std::ostream& operator<<(std::ostream& out, const Ray& ray)
    {
        out << "Direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
        out << "Origin:    (" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << ")" << std::endl;
        out << "Length:    " << ray.length << std::endl;
        return out;
    }
};

struct RayCastResult {
    glm::vec3 point;

    glm::vec3 normal;

    float t;

    bool hit;

    explicit RayCastResult()
        : point(glm::vec3(0.0f))
        , normal(glm::vec3(0, 0, 1))
        , t(-1)
        , hit(false)
    {
    }

    explicit RayCastResult(const glm::vec3& p, const glm::vec3& n, const float distanceOnRay, const bool wasHit)
        : point(p)
        , normal(n)
        , t(distanceOnRay)
        , hit(wasHit)
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
    static bool Intersects(const Sphere& sphere, const Plane& plane)
    {
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

    static bool Intersects(const AABB& box, const Point& point)
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

    static bool Intersects(const Ray& ray, const AABB& box, RayCastResult& result)
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

    static bool Intersects(const Ray& ray, const Sphere& sphere, RayCastResult& result)
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

    static bool Intersects(const Ray& ray, const Plane& plane, RayCastResult& result)
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
};

class BoundingVolumeModelFactory {
public:
    std::unique_ptr<IModel> CreateAABBModel(const AABB& aabb) const
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

    std::unique_ptr<IModel> CreateSphereModel(const Sphere& sphere) const
    {
        const float degreesHorizontal{ 360.0f };
        const float degreesVertical{ 180.0f };
        const int subDivY{ 16 };
        const int subDivZ{ 16 };
        const float radius{ sphere.radius };
        const glm::vec3 positionOffset{ sphere.position };

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        uint32_t indexBase{ 0 };

        float addAngleY{ -degreesHorizontal / static_cast<float>(subDivY) };
        float addAngleZ{ degreesVertical / static_cast<float>(subDivZ) };
        float curAngleY{ 0.0f };
        int stepsY{ 1 };

        while (stepsY <= subDivY) {
            const float sinY = sinf(glm::radians(curAngleY));
            const float cosY = cosf(glm::radians(curAngleY));
            const glm::vec3 directionY(cosY, 0.0f, -sinY);

            const float nextAngleY = curAngleY + addAngleY;
            const float nextSinY = sinf(glm::radians(nextAngleY));
            const float nextCosY = cosf(glm::radians(nextAngleY));
            const glm::vec3 nextDirectionY(nextCosY, 0.0f, -nextSinY);

            float currentAngleZ = 0.0f;
            int stepsZ = 1;
            while (stepsZ <= subDivZ) {
                const float sinZ = sinf(glm::radians(currentAngleZ));
                const float cosZ = cosf(glm::radians(currentAngleZ));

                const float nextAngleZ = currentAngleZ + addAngleZ;
                const float nextSinZ = sinf(glm::radians(nextAngleZ));
                const float nextCosZ = cosf(glm::radians(nextAngleZ));

                const glm::vec3 quadPoints[] = {
                    { directionY.x * sinZ * radius, cosZ * radius, directionY.z * sinZ * radius },
                    { directionY.x * nextSinZ * radius, nextCosZ * radius, directionY.z * nextSinZ * radius },
                    { nextDirectionY.x * nextSinZ * radius, nextCosZ * radius, nextDirectionY.z * nextSinZ * radius },
                    { nextDirectionY.x * sinZ * radius, cosZ * radius, nextDirectionY.z * sinZ * radius }
                };

                for (const auto pt : quadPoints) {
                    vertices.push_back(positionOffset + pt);
                }

                const uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
                for (const auto idx : quadIndices) {
                    indices.push_back(indexBase + idx);
                }
                indexBase += 6;

                stepsZ++;
                currentAngleZ += addAngleZ;
            }
            stepsY++;
            curAngleY += addAngleY;
        }

        auto allocator = AllocatorProvider::Instance().GetAllocator();
        auto vertexBuffer = std::make_unique<VBO>(*allocator);
        vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
        auto indexBuffer = std::make_unique<IBO>(*allocator);
        indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));
        return std::make_unique<Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
    }
};

enum class BoundingVolumeType {
    SPHERE = 0,
    AABB
};

class IBoundingVolumeComponent {
public:
    virtual bool IsInFrustum(const Frustum& frustum) = 0;

    virtual bool Intersects(const Ray& ray, RayCastResult& result) = 0;

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

    bool Intersects(const Ray& ray, RayCastResult& result) override
    {
        return Culling::Intersects(ray, m_working, result);
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
        BoundingVolumeModelFactory modelFactory{};
        m_model = modelFactory.CreateAABBModel(m_working);
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
    std::shared_ptr<IModel> m_model;
#endif
    AABB m_original;

    std::vector<glm::vec3> m_originalAABBPoints;

    AABB m_working;

    std::vector<glm::vec3> m_vorkingAABBPoints;
};

class SphereBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    SphereBoundingVolumeComponent(const Sphere& sphere, const float scale)
        : m_original(sphere)
        , m_working(sphere)
        , m_scale(scale)
    {
    }

    ~SphereBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const Frustum& frustum) override
    {
        return Culling::Intersects(frustum, m_working);
    }

    bool Intersects(const Ray& ray, RayCastResult& result) override
    {
        return Culling::Intersects(ray, m_working, result);
    }

    void Update(const glm::mat4& worldTransform) override
    {
        m_working.position = worldTransform * glm::vec4(m_original.position, 1.0f);
        m_working.radius = m_original.radius * m_scale;
#ifdef RENDER_BOUNDING_VOLUMES
        BoundingVolumeModelFactory modelFactory{};
        m_model = modelFactory.CreateSphereModel(m_working);
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
    std::shared_ptr<IModel> m_model;
#endif
    Sphere m_original;

    Sphere m_working;

    float m_scale;
};

class BoundingVolumeComponentFactory {
public:
    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const std::vector<glm::vec3>& vertices) const
    {
        const auto aabb = CreateABBFromVertices(vertices);
        return std::make_unique<AABBBoundingVolumeComponent>(aabb);
    }

    // TODO -> scale must not change over time
    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const std::vector<glm::vec3>& vertices, const float scale) const
    {
        const auto aabb = CreateABBFromVertices(vertices);

        float maxExtent = std::numeric_limits<float>::min();
        const auto boxHalfExtents = aabb.GetHalfSize();
        for (auto i = 0; i < boxHalfExtents.length(); i++) {
            maxExtent = std::max(maxExtent, boxHalfExtents[i]);
        }

        const Sphere sphere{ aabb.GetCenter(), maxExtent };

        return std::make_unique<SphereBoundingVolumeComponent>(sphere, scale);
    }

private:
    AABB CreateABBFromVertices(const std::vector<glm::vec3>& vertices) const
    {
        AABB aabb{};
        for (const auto& v : vertices) {
            for (auto i = 0; i < aabb.minExtents.length(); i++) {
                aabb.minExtents[i] = std::min(aabb.minExtents[i], v[i]);
                aabb.maxExtents[i] = std::max(aabb.maxExtents[i], v[i]);
            }
        }
        return aabb;
    }
};

#endif // __CULLING_H__
