#ifndef __ILIGHT_COMPONENT_H__
#define __ILIGHT_COMPONENT_H__

#include "../../render/ViewFrustum.h"

namespace prev_test::component::light {
class ILightComponent {
public:
    virtual glm::mat4 LookAt() const = 0;

    virtual glm::mat4 GetProjectionMatrix() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual glm::vec3 GetDirection() const = 0;

    virtual glm::vec3 GetColor() const = 0;

    virtual void SetColor(const glm::vec3& color) = 0;

    virtual glm::vec3 GetAttenuation() const = 0;

    virtual void SetAttenuation(const glm::vec3& attenuation) = 0;

    virtual const prev_test::render::ViewFrustum& GetViewFrustum() const = 0;

    virtual void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) = 0;

public:
    virtual ~ILightComponent() = default;
};
} // namespace prev_test::component::light

#endif // __ILIGHT_COMPONENT_H__