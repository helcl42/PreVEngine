#include "Frustum.h"

#include "../MathUtils.h"
#include "../Utils.h"

namespace prev::util::intersection {
Frustum::Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    const auto viewProjectionMatrix{ projectionMatrix * viewMatrix };
    const auto inverseViewProjectionMatrix{ glm::inverse(viewProjectionMatrix) };

    const glm::vec3 col1{ viewProjectionMatrix[0].x, viewProjectionMatrix[1].x, viewProjectionMatrix[2].x };
    const glm::vec3 col2{ viewProjectionMatrix[0].y, viewProjectionMatrix[1].y, viewProjectionMatrix[2].y };
    const glm::vec3 col3{ viewProjectionMatrix[0].z, viewProjectionMatrix[1].z, viewProjectionMatrix[2].z };
    const glm::vec3 col4{ viewProjectionMatrix[0].w, viewProjectionMatrix[1].w, viewProjectionMatrix[2].w };

    const float M4x{ viewProjectionMatrix[3].x }; // D component for X (related to Left/Right)
    const float M4y{ viewProjectionMatrix[3].y }; // D component for Y (related to Top/Bottom)
    const float M4z{ viewProjectionMatrix[3].z }; // D component for Z (related to Near/Far)
    const float M4w{ viewProjectionMatrix[3].w }; // The M_ww term (related to all planes)

    planes[0] = Plane{ col4 + col1, -(M4w + M4x) }; // left
    planes[1] = Plane{ col4 - col1, -(M4w - M4x) }; // right
    planes[2] = Plane{ col4 - col2, -(M4w - M4y) }; // top
    planes[3] = Plane{ col4 + col2, -(M4w + M4y) }; // bottom
    planes[4] = Plane{ col4 + col3, -(M4w + M4z) }; // front - near
    planes[5] = Plane{ col4 - col3, -(M4w - M4z) }; // back - far

    const auto frustumCorners{ prev::util::math::GetFrustumCorners(inverseViewProjectionMatrix) };
    for (size_t i = 0; i < frustumCorners.size(); ++i) {
        points[i] = Point(frustumCorners[i]);
    }
}

float Frustum::GetRadius() const
{
    const auto center{ GetCenter() };

    float radius{ 0.0f };
    for (size_t i = 0; i < points.size(); ++i) {
        const float distance{ glm::length(points[i].position - center.position) };
        radius = glm::max(radius, distance);
    }
    return radius;
}

Point Frustum::GetCenter() const
{
    glm::vec3 frustumCenter{ 0.0f };
    for (size_t i = 0; i < points.size(); ++i) {
        frustumCenter += points[i].position;
    }
    frustumCenter /= static_cast<float>(points.size());
    return { frustumCenter };
}

std::vector<glm::vec3> Frustum::GetPoints() const
{
    std::vector<glm::vec3> result(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        result[i] = points[i].position;
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const Frustum& frustum)
{
    out << "Planes:" << std::endl;
    for (const auto& plane : frustum.planes) {
        out << plane << std::endl;
    }
    out << "Points:" << std::endl;
    for (const auto& point : frustum.points) {
        out << point << std::endl;
    }
    return out;
}
} // namespace prev::util::intersection
