#ifndef __MOUSE_RAY_CASTER_COMPONENT_H__
#define __MOUSE_RAY_CASTER_COMPONENT_H__

#include "../../common/intersection/Ray.h"
#include "../../render/IModel.h"
#include "IMouseRayCasterComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/window/WindowEvents.h>

namespace prev_test::component::ray_casting {
class MouseRayCasterComponent final : public IMouseRayCasterComponent {
public:
    MouseRayCasterComponent() = default;

    ~MouseRayCasterComponent() = default;

public:
    void Update(float deltaTime) override;

    void SetViewPortDimensions(const glm::vec2& viewPortDimensions) override;

    void SetViewMatrix(const glm::mat4& viewMatrix) override;

    void SetProjectionMatrix(const glm::mat4& projectionMatrix) override;

    void SetRayDirection(const glm::vec3& rayDirection) override;

    void SetRayLength(const float len) override;

    void SetRayStartPosition(const glm::vec3& position) override;

    void SetOrientationOffsetAngles(const glm::vec2& angles) override;

    prev_test::common::intersection::Ray GetRay() const override;

#ifdef RENDER_RAYCASTS
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
public:
    void operator()(const prev::input::mouse::MouseEvent& moveEvent);

    void operator()(const prev::window::WindowResizeEvent& vwindowResizeEvent);

protected:
    glm::mat4 m_viewMatrix;

    glm::mat4 m_projectionMatrix;

    glm::vec2 m_currentMousePosition;

    glm::vec3 m_rayDirection;

    glm::vec3 m_rayStartPosition;

    float m_rayLength{ 500.0f };

    glm::vec2 m_orientationOffsetAngles{ 0.0f, 0.0f };

    glm::vec2 m_viewPortDimensions{ 0.0f, 0.0f };
#ifdef RENDER_RAYCASTS
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
private:
    prev::event::EventHandler<MouseRayCasterComponent, prev::input::mouse::MouseEvent> m_mouseMoveHandler{ *this };

    prev::event::EventHandler<MouseRayCasterComponent, prev::window::WindowResizeEvent> m_viewFrustumChangeHandler{ *this };
};
} // namespace prev_test::component::ray_casting

#endif