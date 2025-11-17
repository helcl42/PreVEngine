#ifndef __TRANSFORM_FACTORY_H__
#define __TRANSFORM_FACTORY_H__

#include "ITransformComponent.h"

namespace prev_test::component::transform {
class TrasnformComponentFactory {
public:
    std::unique_ptr<ITransformComponent> Create() const;

    std::unique_ptr<ITransformComponent> Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) const;
};
} // namespace prev_test::component::transform

#endif