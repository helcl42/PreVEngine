#include "TransformComponentFactory.h"

#include "TransformComponent.h"

namespace prev_test::component::transform {
std::unique_ptr<ITransformComponent> TrasnformComponentFactory::Create() const
{
    return std::make_unique<TransformComponent>();
}

std::unique_ptr<ITransformComponent> TrasnformComponentFactory::Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) const
{
    return std::make_unique<TransformComponent>(position, orientation, scale);
}
} // namespace prev_test::component::transform