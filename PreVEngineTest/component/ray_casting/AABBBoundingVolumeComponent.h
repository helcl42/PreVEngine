#ifndef __AABB_BOUNDING_VOLUME_COMPONENT_H__
#define __AABB_BOUNDING_VOLUME_COMPONENT_H__

#include "IBoundingVolumeComponent.h"
#include "RayCastingCommon.h"

#include "../../common/intersection/AABB.h"
#include "../../common/intersection/Frustum.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class AABBBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    AABBBoundingVolumeComponent(const prev_test::common::intersection::AABB& box, const float scale, const glm::vec3& offset);

    ~AABBBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const prev_test::common::intersection::Frustum& frustum) override;

    bool Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result) override;

    void Update(const glm::mat4& worldTransform) override;

    BoundingVolumeType GetType() const override;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    static prev_test::common::intersection::AABB ScaleBox(const prev_test::common::intersection::AABB& box, const float scale);

    static prev_test::common::intersection::AABB OffsetBox(const prev_test::common::intersection::AABB& box, const glm::vec3& offset);

private:
    const float m_scale;

    const glm::vec3 m_offset;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
    prev_test::common::intersection::AABB m_original;

    std::vector<glm::vec3> m_originalAABBPoints;

    prev_test::common::intersection::AABB m_working;

    std::vector<glm::vec3> m_vorkingAABBPoints;
};
} // namespace prev_test::component::ray_casting

#endif // !__AABB_BOUNDING_VOLUME_COMPONENT_H__
