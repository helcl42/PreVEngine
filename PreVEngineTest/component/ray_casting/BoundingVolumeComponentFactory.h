#ifndef __BOUNDING_VOLUME_COMPONENT_FACTORY_H__
#define __BOUNDING_VOLUME_COMPONENT_FACTORY_H__

#include "IBoundingVolumeComponent.h"

#include "../../common/intersection/AABB.h"

namespace prev_test::component::ray_casting {
class BoundingVolumeComponentFactory final {
public:
    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const std::vector<glm::vec3>& vertices, const float scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f)) const;

    // TODO -> scale must not change over time
    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const std::vector<glm::vec3>& vertices, const float scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f)) const;

private:
    prev_test::common::intersection::AABB CreateABBFromVertices(const std::vector<glm::vec3>& vertices) const;
};
} // namespace prev_test::component::ray_casting

#endif