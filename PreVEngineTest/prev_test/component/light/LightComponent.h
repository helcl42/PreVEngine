#ifndef __LIGHT_COMPONENT_H__
#define __LIGHT_COMPONENT_H__

#include "ILightComponent.h"

namespace prev_test::component::light {
class LightComponent : public ILightComponent {
public:
    LightComponent(const glm::vec3& pos);

    LightComponent(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& attenuation);

    ~LightComponent() = default;

public:
    glm::mat4 LookAt() const override;

    glm::mat4 GetProjectionMatrix() const override;

    glm::vec3 GetPosition() const override;

    void SetPosition(const glm::vec3& position) override;

    glm::vec3 GetDirection() const override;

    glm::vec3 GetColor() const override;

    void SetColor(const glm::vec3& color) override;

    glm::vec3 GetAttenuation() const override;

    void SetAttenuation(const glm::vec3& attenuation) override;

    const prev_test::render::ViewFrustum& GetViewFrustum() const override;

    void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) override;

private:
    static const inline float NEAR_CLIPPING_PLANE{ 1.0f };

    static const inline float FAR_CLIPPING_PLANE{ 300.0f };

private:
    glm::vec3 m_lookAtPosition{ 0.0f, 0.0f, 0.0f };

    glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

    prev_test::render::ViewFrustum m_viewFrustum{ 45.0f, REVERSE_DEPTH ? FAR_CLIPPING_PLANE : NEAR_CLIPPING_PLANE, REVERSE_DEPTH ? NEAR_CLIPPING_PLANE : FAR_CLIPPING_PLANE };

    glm::vec3 m_position;

    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

    glm::vec3 m_attenuation{ 1.0f, 0.0f, 0.0f };
};
} // namespace prev_test::component::light

#endif // !__LIGHT_COMPONENT_H__
