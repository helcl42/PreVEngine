#ifndef __XR_EYES_H__
#define __XR_EYES_H__

#ifdef ENABLE_XR

#include "../component/camera/ICameraComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/xr/XrEvents.h>

#include <memory>
#include <optional>
#include <vector>

namespace prev_test::scene {
// While an immersive session runs, the headset places the eyes of its camera. The camera adds it as a child, and a
// node updates its children after itself, so the eyes replace the views the camera has just set.
class XrEyes final : public prev::scene::graph::SceneNode {
public:
    explicit XrEyes(std::vector<std::shared_ptr<prev_test::component::camera::ICameraComponent>> cameraComponents);

    ~XrEyes() = default;

public:
    void Update(float deltaTime) override;

public:
    void operator()(const prev::xr::CameraEvent& cameraEvent);

    void operator()(const prev::xr::XrSessionChangedEvent& sessionEvent);

private:
    std::vector<std::shared_ptr<prev_test::component::camera::ICameraComponent>> m_cameraComponents; // the camera's
    std::optional<prev::xr::CameraEvent> m_eyes; // the headset's latest, while an immersive session runs

    prev::event::EventHandler<XrEyes, prev::xr::CameraEvent> m_cameraEventHandler{ *this };
    prev::event::EventHandler<XrEyes, prev::xr::XrSessionChangedEvent> m_sessionHandler{ *this };
};
} // namespace prev_test::scene

#endif // ENABLE_XR

#endif // !__XR_EYES_H__
