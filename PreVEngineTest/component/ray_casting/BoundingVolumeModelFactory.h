#ifndef __BOUNDING_VOLUME_MODEL_FACTORY_H__
#define __BOUNDING_VOLUME_MODEL_FACTORY_H__

#include "../../common/intersection/AABB.h"
#include "../../common/intersection/OBB.h"
#include "../../common/intersection/Sphere.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class BoundingVolumeModelFactory final {
public:
    std::unique_ptr<prev_test::render::IModel> CreateAABBModel(const prev_test::common::intersection::AABB& aabb) const;

    std::unique_ptr<prev_test::render::IModel> CreateOBBModel(const prev_test::common::intersection::OBB& obb) const;

    std::unique_ptr<prev_test::render::IModel> CreateSphereModel(const prev_test::common::intersection::Sphere& sphere) const;
};
} // namespace prev_test::component::ray_casting

#endif