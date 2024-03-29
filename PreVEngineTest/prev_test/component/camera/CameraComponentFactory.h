#ifndef __CAMERA_COMPONENT_FACTORY_H__
#define __CAMERA_COMPONENT_FACTORY_H__

#include "ICameraComponent.h"

#include <memory>

namespace prev_test::component::camera {
class CameraComponentFactory {
public:
    std::unique_ptr<ICameraComponent> Create(const glm::quat& orient, const glm::vec3& pos, const bool useFixedUp) const;
};
} // namespace prev_test::component::camera

#endif // !__CAMERA_COMPONENT_FACTORY_H__
