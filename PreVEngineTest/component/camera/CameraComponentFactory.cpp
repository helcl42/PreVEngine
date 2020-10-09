#include "CameraComponentFactory.h"
#include "CameraComponent.h"

namespace prev_test::component::camera {
std::unique_ptr<ICameraComponent> CameraComponentFactory::Create(const glm::quat& orient, const glm::vec3& pos) const
{
    return std::make_unique<CameraComponent>(orient, pos);
}
} // namespace prev_test::component::camera