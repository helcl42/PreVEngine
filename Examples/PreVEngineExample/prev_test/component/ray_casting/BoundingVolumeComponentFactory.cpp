#include "BoundingVolumeComponentFactory.h"
#include "AABBBoundingVolumeComponent.h"
#include "OBBBoundingVolumeComponent.h"
#include "SphereBoundingVolumeComponent.h"

#include "../../render/mesh/MeshUtil.h"

namespace prev_test::component::ray_casting {
BoundingVolumeComponentFactory::BoundingVolumeComponentFactory(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateAABB(const prev::util::intersection::AABB& aabb, const glm::vec3& scale, const glm::vec3& offset) const
{
    return std::make_unique<AABBBoundingVolumeComponent>(m_allocator, aabb, scale, offset);
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateAABB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale, const glm::vec3& offset) const
{
    const auto vertices{ prev_test::render::mesh::MeshUtil::GetMeshTransformedVertices(mesh) };
    const auto aabb{ CreateAABBFromVertices(vertices) };
    return CreateAABB(aabb, scale, offset);
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateOBB(const prev::util::intersection::OBB& obb, const glm::vec3& scale, const glm::vec3& offset) const
{
    return std::make_unique<OBBBoundingVolumeComponent>(m_allocator, obb, scale, offset);
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateOBB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale, const glm::vec3& offset) const
{
    const auto vertices{ prev_test::render::mesh::MeshUtil::GetMeshTransformedVertices(mesh) };
    const auto aabb{ CreateAABBFromVertices(vertices) };
    const auto obb{ CreateOBBFromAABB(aabb) };
    return CreateOBB(obb, scale, offset);
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateSphere(const prev::util::intersection::Sphere& sphere, const float scale, const glm::vec3& offset) const
{
    return std::make_unique<SphereBoundingVolumeComponent>(m_allocator, sphere, scale, offset);
}

std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateSphere(const std::shared_ptr<prev_test::render::IMesh>& mesh, const float scale, const glm::vec3& offset) const
{
    const auto vertices{ prev_test::render::mesh::MeshUtil::GetMeshTransformedVertices(mesh) };
    const auto aabb{ CreateAABBFromVertices(vertices) };
    const auto sphere{ CreateSphereFromAABB(aabb) };
    return CreateSphere(sphere, scale, offset);
}

prev::util::intersection::AABB BoundingVolumeComponentFactory::CreateAABBFromVertices(const std::vector<glm::vec3>& vertices) const
{
    prev::util::intersection::AABB aabb{};
    for (const auto& v : vertices) {
        aabb.minExtents = glm::min(aabb.minExtents, v);
        aabb.maxExtents = glm::max(aabb.maxExtents, v);
    }
    return aabb;
}

prev::util::intersection::OBB BoundingVolumeComponentFactory::CreateOBBFromVertices(const std::vector<glm::vec3>& vertices) const
{
    // https://stackoverflow.com/a/6189655
    glm::vec3 centroid{ 0.0f };
    for (const auto& v : vertices) {
        centroid += v;
    }
    centroid /= static_cast<float>(vertices.size());

    glm::mat3 covarianceMatrix{ 0.0f };
    for (const auto& v : vertices) {
        covarianceMatrix += glm::outerProduct(v - centroid, v - centroid);
    }

    glm::vec3 averageDistancesFromCentroid{ 0.0 };
    for (const auto& v : vertices) {
        for (glm::length_t i = 0; i < averageDistancesFromCentroid.length(); i++) {
            averageDistancesFromCentroid[i] += glm::distance(v[i], centroid[i]);
        }
    }
    averageDistancesFromCentroid /= static_cast<float>(vertices.size());

    prev::util::intersection::OBB obb{ glm::normalize(glm::quat_cast(glm::mat3(glm::normalize(covarianceMatrix[0]), glm::normalize(covarianceMatrix[1]), glm::normalize(covarianceMatrix[2])))), centroid, averageDistancesFromCentroid };
    return obb;
}

prev::util::intersection::Sphere BoundingVolumeComponentFactory::CreateSphereFromVertices(const std::vector<glm::vec3>& vertices) const
{
    glm::vec3 centroid{ 0.0f };
    for (const auto& v : vertices) {
        centroid += v;
    }
    centroid /= static_cast<float>(vertices.size());

    float averageDistancesFromCentroid{ 0.0 };
    for (const auto& v : vertices) {
        averageDistancesFromCentroid += glm::distance(v, centroid);
    }
    averageDistancesFromCentroid /= static_cast<float>(vertices.size());

    return prev::util::intersection::Sphere{ centroid, averageDistancesFromCentroid };
}

prev::util::intersection::OBB BoundingVolumeComponentFactory::CreateOBBFromAABB(const prev::util::intersection::AABB& box) const
{
    return prev::util::intersection::OBB{ glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f }, box.GetCenter(), box.GetHalfSize() };
}

prev::util::intersection::Sphere BoundingVolumeComponentFactory::CreateSphereFromAABB(const prev::util::intersection::AABB& box) const
{
    const auto boxHalfExtents{ box.GetHalfSize() };
    float maxExtent{ -std::numeric_limits<float>::max() };
    for (auto i = 0; i < boxHalfExtents.length(); i++) {
        maxExtent = std::max(maxExtent, boxHalfExtents[i]);
    }
    return prev::util::intersection::Sphere{ box.GetCenter(), maxExtent };
}
} // namespace prev_test::component::ray_casting