#ifndef __SANDBOX_CAMERA_FACTORY_H__
#define __SANDBOX_CAMERA_FACTORY_H__

#include "Camera.h"

#include <cstdint>
#include <memory>

namespace sandbox::scene::camera {
class CameraFactory {
public:
    static std::unique_ptr<Camera> Create(uint32_t viewCount);
};
} // namespace sandbox::scene::camera

#endif // !__SANDBOX_CAMERA_FACTORY_H__
