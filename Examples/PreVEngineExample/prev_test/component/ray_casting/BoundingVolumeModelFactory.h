#ifndef __BOUNDING_VOLUME_MODEL_FACTORY_H__
#define __BOUNDING_VOLUME_MODEL_FACTORY_H__

#include "../../render/IModel.h"

#include <prev/util/intersection/AABB.h>
#include <prev/util/intersection/OBB.h>
#include <prev/util/intersection/Sphere.h>

namespace prev_test::component::ray_casting {
class BoundingVolumeModelFactory final {
public:
    BoundingVolumeModelFactory(prev::core::memory::Allocator& allocator);

    ~BoundingVolumeModelFactory() = default;

public:
    std::unique_ptr<prev_test::render::IModel> CreateAABBModel(const prev::util::intersection::AABB& aabb) const;

    std::unique_ptr<prev_test::render::IModel> CreateAABBModel(const prev::util::intersection::AABB& aabb, const std::shared_ptr<prev_test::render::IModel>& model) const;

    std::unique_ptr<prev_test::render::IModel> CreateOBBModel(const prev::util::intersection::OBB& obb) const;

    std::unique_ptr<prev_test::render::IModel> CreateOBBModel(const prev::util::intersection::OBB& obb, const std::shared_ptr<prev_test::render::IModel>& model) const;

    std::unique_ptr<prev_test::render::IModel> CreateSphereModel(const prev::util::intersection::Sphere& sphere) const;

    std::unique_ptr<prev_test::render::IModel> CreateSphereModel(const prev::util::intersection::Sphere& sphere, const std::shared_ptr<prev_test::render::IModel>& model) const;

private:
    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::ray_casting

#endif