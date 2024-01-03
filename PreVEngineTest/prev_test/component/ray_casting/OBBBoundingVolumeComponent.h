#ifndef __OBB_BOUNDING_VOLUME_COMPONENT_H__
#define __OBB_BOUNDING_VOLUME_COMPONENT_H__

#include "IBoundingVolumeComponent.h"
#include "RayCastingCommon.h"

#include "../../common/intersection/Frustum.h"
#include "../../common/intersection/OBB.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class OBBBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    OBBBoundingVolumeComponent(const prev_test::common::intersection::OBB& obb, const glm::vec3& scale, const glm::vec3& offset);

    ~OBBBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const prev_test::common::intersection::Frustum& frustum) override;

    bool Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result) override;

    void Update(const glm::mat4& worldTransform) override;

    BoundingVolumeType GetType() const override;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    static prev_test::common::intersection::OBB ScaleOBB(const prev_test::common::intersection::OBB& obb, const glm::vec3& scale);

    static prev_test::common::intersection::OBB OffsetOBB(const prev_test::common::intersection::OBB& obb, const glm::vec3& offset);

private:
    const glm::vec3& m_scale;

    const glm::vec3 m_offset;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
    prev_test::common::intersection::OBB m_original;

    prev_test::common::intersection::OBB m_working;
};
} // namespace prev_test::component::ray_casting

#endif // !__AABB_BOUNDING_VOLUME_COMPONENT_H__
