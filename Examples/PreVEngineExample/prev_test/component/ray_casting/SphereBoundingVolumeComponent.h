#ifndef __SPHERE_BOUNDING_VOLUME_COMPONENT_H__
#define __SPHERE_BOUNDING_VOLUME_COMPONENT_H__

#include "IBoundingVolumeComponent.h"
#include "RayCastingCommon.h"

#include "../../render/IModel.h"

#include <prev/core/device/Device.h>
#include <prev/util/intersection/Frustum.h>
#include <prev/util/intersection/Sphere.h>

namespace prev_test::component::ray_casting {
class SphereBoundingVolumeComponent : public IBoundingVolumeComponent {
public:
    SphereBoundingVolumeComponent(const prev::core::device::Device& device, const prev::util::intersection::Sphere& sphere, const float scale, const glm::vec3& offset);

    ~SphereBoundingVolumeComponent() = default;

public:
    bool IsInFrustum(const prev::util::intersection::Frustum& frustum) override;

    bool Intersects(const prev::util::intersection::Ray& ray, prev::util::intersection::RayCastResult& result) override;

    void Update(const glm::mat4& worldTransform) override;

    BoundingVolumeType GetType() const override;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    static prev::util::intersection::Sphere ScaleSphere(const prev::util::intersection::Sphere& sphere, const float scale);

    static prev::util::intersection::Sphere OffsetSphere(const prev::util::intersection::Sphere& sphere, const glm::vec3& offset);

private:

    const prev::core::device::Device& m_device;

    float m_scale;

    glm::vec3 m_offset;

#ifdef RENDER_BOUNDING_VOLUMES
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
    prev::util::intersection::Sphere m_original;

    prev::util::intersection::Sphere m_working;
};
} // namespace prev_test::component::ray_casting

#endif