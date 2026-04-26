#ifndef __RAY_MODEL_FACTORY_H__
#define __RAY_MODEL_FACTORY_H__

#include "../../render/IModel.h"

#include <prev/util/intersection/Ray.h>

namespace prev_test::component::ray_casting {
class RayModelFactory final {
public:
    RayModelFactory(const prev::core::device::Device& device);

    ~RayModelFactory() = default;

public:
    std::unique_ptr<prev_test::render::IModel> Create(const prev::util::intersection::Ray& ray) const;

private:
    const prev::core::device::Device& m_device;
};
} // namespace prev_test::component::ray_casting

#endif // !__RAY_MODEL_FACTORY_H__
