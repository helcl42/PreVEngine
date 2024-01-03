#ifndef __RAY_MODEL_FACTORY_H__
#define __RAY_MODEL_FACTORY_H__

#include "../../common/intersection/Ray.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class RayModelFactory final {
public:
    std::unique_ptr<prev_test::render::IModel> Create(const prev_test::common::intersection::Ray& ray) const;
};
} // namespace prev_test::component::ray_casting

#endif // !__RAY_MODEL_FACTORY_H__
