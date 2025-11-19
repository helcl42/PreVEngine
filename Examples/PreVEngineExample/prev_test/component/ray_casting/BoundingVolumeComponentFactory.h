#ifndef __BOUNDING_VOLUME_COMPONENT_FACTORY_H__
#define __BOUNDING_VOLUME_COMPONENT_FACTORY_H__

#include "IBoundingVolumeComponent.h"

#include <prev/core/memory/Allocator.h>
#include <prev/util/intersection/AABB.h>
#include <prev/util/intersection/OBB.h>
#include <prev/util/intersection/Sphere.h>

namespace prev_test::component::ray_casting {
class BoundingVolumeComponentFactory final {
public:
    BoundingVolumeComponentFactory(prev::core::memory::Allocator& allocator);

    ~BoundingVolumeComponentFactory() = default;

public:
    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const prev::util::intersection::AABB& aabb, const glm::vec3& scale, const glm::vec3& offset) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateAABB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale = glm::vec3{ 1.0f }, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateOBB(const prev::util::intersection::OBB& obb, const glm::vec3& scale = glm::vec3{ 1.0f }, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateOBB(const std::shared_ptr<prev_test::render::IMesh>& mesh, const glm::vec3& scale = glm::vec3{ 1.0f }, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const prev::util::intersection::Sphere& sphere, const float scale, const glm::vec3& offset) const;

    std::unique_ptr<IBoundingVolumeComponent> CreateSphere(const std::shared_ptr<prev_test::render::IMesh>& mesh, const float scale = 1.0f, const glm::vec3& offset = glm::vec3{ 0.0f }) const;

private:
    prev::util::intersection::AABB CreateAABBFromVertices(const std::vector<glm::vec3>& vertices) const;

    prev::util::intersection::OBB CreateOBBFromVertices(const std::vector<glm::vec3>& vertices) const;

    prev::util::intersection::Sphere CreateSphereFromVertices(const std::vector<glm::vec3>& vertices) const;

    prev::util::intersection::OBB CreateOBBFromAABB(const prev::util::intersection::AABB& box) const;

    prev::util::intersection::Sphere CreateSphereFromAABB(const prev::util::intersection::AABB& box) const;

private:
    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::ray_casting

#endif