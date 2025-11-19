#ifndef __OBB_BOUNDING_VOLUME_COMPONENT_H__
#define __OBB_BOUNDING_VOLUME_COMPONENT_H__

#include "IBoundingVolumeComponent.h"
#include "RayCastingCommon.h"

#include "../../render/IModel.h"

#include <prev/util/intersection/Frustum.h>
#include <prev/util/intersection/OBB.h>

namespace prev_test::component::ray_casting {
class OBBBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    OBBBoundingVolumeComponent(prev::core::memory::Allocator& allocator, const prev::util::intersection::OBB& obb, const glm::vec3& scale, const glm::vec3& offset);

    ~OBBBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const prev::util::intersection::Frustum& frustum) override;

    bool Intersects(const prev::util::intersection::Ray& ray, prev::util::intersection::RayCastResult& result) override;

    void Update(const glm::mat4& worldTransform) override;

    BoundingVolumeType GetType() const override;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    static prev::util::intersection::OBB ScaleOBB(const prev::util::intersection::OBB& obb, const glm::vec3& scale);

    static prev::util::intersection::OBB OffsetOBB(const prev::util::intersection::OBB& obb, const glm::vec3& offset);

private:
    prev::core::memory::Allocator& m_allocator;

    glm::vec3 m_scale;

    glm::vec3 m_offset;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
    prev::util::intersection::OBB m_original;

    prev::util::intersection::OBB m_working;
};
} // namespace prev_test::component::ray_casting

#endif // !__AABB_BOUNDING_VOLUME_COMPONENT_H__
