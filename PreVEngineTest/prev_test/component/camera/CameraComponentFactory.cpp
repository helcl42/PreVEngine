#include "CameraComponentFactory.h"
#include "CameraComponent.h"

namespace prev_test::component::camera {
std::unique_ptr<ICameraComponent> CameraComponentFactory::Create(const glm::quat& orient, const glm::vec3& pos, const bool useFixedUp) const
{
    return std::make_unique<CameraComponent>(orient, pos, useFixedUp);
}
} // namespace prev_test::component::camera