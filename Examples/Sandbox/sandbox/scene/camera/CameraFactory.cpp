#include "CameraFactory.h"

#ifdef ENABLE_XR
#include "XrCamera.h"
#else
#include "DefaultCamera.h"
#endif

namespace sandbox::scene::camera {
std::unique_ptr<Camera> CameraFactory::Create(uint32_t viewCount)
{
#ifdef ENABLE_XR
    return std::make_unique<XrCamera>(viewCount);
#else
    return std::make_unique<DefaultCamera>(viewCount);
#endif
}
} // namespace sandbox::scene::camera
