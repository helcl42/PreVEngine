#ifndef __BOUNDING_VOLUME_COMPONENT_FACTORY_H__
#define __BOUNDING_VOLUME_COMPONENT_FACTORY_H__

#include "IBoundingVolumeComponent.h"

#include "../../common/intersection/AABB.h"
#include "../../common/intersection/OBB.h"

namespace prev_test::component::ray_casting {
class BoundingVolumeComponentFactory final {
public:
    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale = glm::vec3{ 1.0f }, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateOBB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale = glm::vec3{ 1.0f }, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const std::shared_ptr<prev_test::render::IMesh>& mesh, const float scale = 1.0f, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

private:
    prev_test::common::intersection::AABB CreateAABBFromVertices(const std::vector<glm::vec3>& vertices) const;

    prev_test::common::intersection::OBB CreateOBBFromVertices(const std::vector<glm::vec3>& vertices) const;
};
} // namespace prev_test::component::ray_casting

#endif