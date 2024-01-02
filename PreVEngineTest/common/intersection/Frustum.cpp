#include "Frustum.h"

#include <prev/util/MathUtils.h>
#include <prev/util/Utils.h>

namespace prev_test::common::intersection {
Frustum::Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    const auto viewProjectionMatrix{ projectionMatrix * viewMatrix };
    const auto inverseViewProjectionMatrix{ glm::inverse(viewProjectionMatrix) };

    const glm::vec3 col1{ viewProjectionMatrix[0].x, viewProjectionMatrix[1].x, viewProjectionMatrix[2].x };
    const glm::vec3 col2{ viewProjectionMatrix[0].y, viewProjectionMatrix[1].y, viewProjectionMatrix[2].y };
    const glm::vec3 col3{ viewProjectionMatrix[0].z, viewProjectionMatrix[1].z, viewProjectionMatrix[2].z };
    const glm::vec3 col4{ viewProjectionMatrix[0].w, viewProjectionMatrix[1].w, viewProjectionMatrix[2].w };

    planes[0] = Plane{ col4 + col1, viewProjectionMatrix[3].w + viewProjectionMatrix[3].x }; // left
    planes[1] = Plane{ col4 - col1, viewProjectionMatrix[3].w - viewProjectionMatrix[3].x }; // right
    planes[2] = Plane{ col4 - col2, viewProjectionMatrix[3].w - viewProjectionMatrix[3].y }; // top
    planes[3] = Plane{ col4 + col2, viewProjectionMatrix[3].w + viewProjectionMatrix[3].y }; // bottom
    planes[4] = Plane{ col4 + col3, viewProjectionMatrix[3].w + viewProjectionMatrix[3].z }; // front - near
    planes[5] = Plane{ col4 - col3, viewProjectionMatrix[3].w - viewProjectionMatrix[3].z }; // back - far

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
} // namespace prev_test::common::intersection
