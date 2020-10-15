#ifndef __IBOUNDING_VOLUME_COMPONENT_H__
#define __IBOUNDING_VOLUME_COMPONENT_H__

#include "../../common/intersection/Frustum.h"
#include "../../common/intersection/Ray.h"
#include "../../common/intersection/RayCastResult.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
enum class BoundingVolumeType {
    SPHERE = 0,
    AABB
};

class IBoundingVolumeComponent {
public:
    virtual bool IsInFrustum(const prev_test::common::intersection::Frustum& frustum) = 0;

    virtual bool Intersects(const prev_test::common::intersection::Ray& ray, prev_test::common::intersection::RayCastResult& result) = 0;

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
