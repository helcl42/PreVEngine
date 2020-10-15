#include "BoundingVolumeComponentFactory.h"
#include "AABBBoundingVolumeComponent.h"
#include "SphereBoundingVolumeComponent.h"

#include "../../common/intersection/AABB.h"

namespace prev_test::component::ray_casting {
std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateAABB(const std::vector<glm::vec3>& vertices, const float scale, const glm::vec3& offset) const
{
    const auto aabb = CreateABBFromVertices(vertices);
    return std::make_unique<AABBBoundingVolumeComponent>(aabb, scale, offset);
}

// TODO -> scale must not change over time
std::unique_ptr<IBoundingVolumeComponent> BoundingVolumeComponentFactory::CreateSphere(const std::vector<glm::vec3>& vertices, const float scale, const glm::vec3& offset) const
{
    const auto aabb = CreateABBFromVertices(vertices);

    float maxExtent = std::numeric_limits<float>::min();
    const auto boxHalfExtents = aabb.GetHalfSize();
    for (auto i = 0; i < boxHalfExtents.length(); i++) {
        maxExtent = std::max(maxExtent, boxHalfExtents[i]);
    }

    const prev_test::common::intersection::Sphere sphere{ aabb.GetCenter(), maxExtent };

    return std::make_unique<SphereBoundingVolumeComponent>(sphere, scale, offset);
}

prev_test::common::intersection::AABB BoundingVolumeComponentFactory::CreateABBFromVertices(const std::vector<glm::vec3>& vertices) const
{
    prev_test::common::intersection::AABB aabb{};
    for (const auto& v : vertices) {
        for (auto i = 0; i < aabb.minExtents.length(); i++) {
            aabb.minExtents[i] = std::min(aabb.minExtents[i], v[i]);
            aabb.maxExtents[i] = std::max(aabb.maxExtents[i], v[i]);
        }
    }
    return aabb;
}
} // namespace prev_test::component::ray_casting