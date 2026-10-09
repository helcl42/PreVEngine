#ifndef __SANDBOX_XR_EYES_H__
#define __SANDBOX_XR_EYES_H__

#ifdef ENABLE_XR

#include "../../component/CameraComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/xr/XrEvents.h>

#include <cstdint>
#include <memory>
#include <optional>

namespace sandbox::scene::camera {
// While an immersive session runs, the headset places the eyes of its camera. The camera adds it as a child, and a
// node updates its children after itself, so the eyes replace the views the camera has just set.
class XrEyes final : public prev::scene::graph::SceneNode {
public:
    explicit XrEyes(std::shared_ptr<sandbox::component::CameraComponent> camera);

    ~XrEyes() = default;

public:
    void Update(float deltaTime) override;

public:
    void operator()(const prev::xr::CameraEvent& cameraEvent);

    void operator()(const prev::xr::XrSessionChangedEvent& sessionEvent);

private:
    std::shared_ptr<sandbox::component::CameraComponent> m_camera;
    uint32_t m_maxViews{ 1 };
    std::optional<prev::xr::CameraEvent> m_eyes; // the headset's latest, while an immersive session runs

    prev::event::EventHandler<XrEyes, prev::xr::CameraEvent> m_cameraEventHandler{ *this };
    prev::event::EventHandler<XrEyes, prev::xr::XrSessionChangedEvent> m_sessionHandler{ *this };
};
} // namespace sandbox::scene::camera

#endif // ENABLE_XR

#endif // !__SANDBOX_XR_EYES_H__
