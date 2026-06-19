#ifndef __SANDBOX_XR_CAMERA_H__
#define __SANDBOX_XR_CAMERA_H__

#ifdef ENABLE_XR

#include "Camera.h"

#include <prev/event/EventHandler.h>
#include <prev/xr/XrEvents.h>

#include <cstdint>

namespace sandbox::scene::camera {
class XrCamera final : public Camera {
public:
    explicit XrCamera(uint32_t viewCount);

    ~XrCamera() = default;

public:
    void operator()(const prev::xr::CameraEvent& cameraEvent);

private:
    prev::event::EventHandler<XrCamera, prev::xr::CameraEvent> m_cameraEventHandler{ *this };
};
} // namespace sandbox::scene::camera

#endif // ENABLE_XR

#endif // !__SANDBOX_XR_CAMERA_H__
