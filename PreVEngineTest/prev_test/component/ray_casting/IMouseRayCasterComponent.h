#ifndef __IMOUSE_RAY_CASTER_COMPONENT_H__
#define __IMOUSE_RAY_CASTER_COMPONENT_H__

#include "IRayCasterComponent.h"

namespace prev_test::component::ray_casting {
class IMouseRayCasterComponent : public IRayCasterComponent {
public:
    virtual void SetViewPortDimensions(const glm::vec2& viewPortDimensions) = 0;

    virtual void SetMousePosition(const glm::vec2& mousePosition) = 0;

    virtual void SetViewMatrix(const glm::mat4& viewMatrix) = 0;

    virtual void SetProjectionMatrix(const glm::mat4& projectionMatrix) = 0;

public:
    virtual ~IMouseRayCasterComponent() = default;
};
} // namespace prev_test::component::ray_casting

#endif // !__IMOUSE_RAY_CASTER_COMPONENT_H__
