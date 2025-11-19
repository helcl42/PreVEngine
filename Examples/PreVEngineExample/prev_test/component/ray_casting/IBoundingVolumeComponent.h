#ifndef __IBOUNDING_VOLUME_COMPONENT_H__
#define __IBOUNDING_VOLUME_COMPONENT_H__

#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>
#include <prev/util/intersection/Frustum.h>
#include <prev/util/intersection/Ray.h>
#include <prev/util/intersection/RayCastResult.h>

namespace prev_test::component::ray_casting {
enum class BoundingVolumeType {
    SPHERE = 0,
    AABB,
    OBB
};

class IBoundingVolumeComponent : public prev::scene::component::IComponent {
public:
    virtual bool IsInFrustum(const prev::util::intersection::Frustum& frustum) = 0;

    virtual bool Intersects(const prev::util::intersection::Ray& ray, prev::util::intersection::RayCastResult& result) = 0;

    virtual void Update(const glm::mat4& worldTransform) = 0;

    virtual BoundingVolumeType GetType() const = 0;

#ifdef RENDER_BOUNDING_VOLUMES
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;
#endif

public:
    virtual ~IBoundingVolumeComponent() = default;
};
} // namespace prev_test::component::ray_casting

#endif // !__IBOUNDING_VOLUME_COMPONENT_H__
