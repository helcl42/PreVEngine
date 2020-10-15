#ifndef __SPHERE_BOUNDING_VOLUME_COMPONENT_H__
#define __SPHERE_BOUNDING_VOLUME_COMPONENT_H__

#include "IBoundingVolumeComponent.h"
#include "RayCastingCommon.h"

#include "../../common/intersection/Frustum.h"
#include "../../common/intersection/Sphere.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class SphereBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    SphereBoundingVolumeComponent(const prev_test::common::intersection::Sphere& sphere, const float scale, const glm::vec3& offset);

    ~SphereBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const prev_test::common::intersection::Frustum& frustum) override;

    bool Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result) override;

    void Update(const glm::mat4& worldTransform) override;

    BoundingVolumeType GetType() const override;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    static prev_test::common::intersection::Sphere ScaleSphere(const prev_test::common::intersection::Sphere& sphere, const float scale);

    static prev_test::common::intersection::Sphere OffsetSphere(const prev_test::common::intersection::Sphere& sphere, const glm::vec3& offset);

private:
    const float m_scale;

    const glm::vec3 m_offset;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
    prev_test::common::intersection::Sphere m_original;

    prev_test::common::intersection::Sphere m_working;
};
} // namespace prev_test::component::ray_casting

#endif