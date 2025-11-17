#ifndef __RAY_MODEL_FACTORY_H__
#define __RAY_MODEL_FACTORY_H__

#include "../../common/intersection/Ray.h"
#include "../../render/IModel.h"

#include <prev/core/memory/Allocator.h>

namespace prev_test::component::ray_casting {
class RayModelFactory final {
public:
    RayModelFactory(prev::core::memory::Allocator& allocator);

    ~RayModelFactory() = default;

public:
    std::unique_ptr<prev_test::render::IModel> Create(const prev_test::common::intersection::Ray& ray) const;

private:
    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::ray_casting

#endif // !__RAY_MODEL_FACTORY_H__
