#ifndef __SUN_COMPONENT_H__
#define __SUN_COMPONENT_H__

#include "ISunComponent.h"

namespace prev_test::component::sky {
class SunComponent : public ISunComponent {
public:
    explicit SunComponent(const Flare& flare, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<prev_test::render::IModel>& model);

    virtual ~SunComponent() = default;

public:
    const Flare& GetFlare() const override;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    glm::vec2 ComputeFlarePosition(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const override;

private:
    Flare m_flare;

    std::shared_ptr<prev_test::render::IMaterial> m_material;

    std::shared_ptr<prev_test::render::IModel> m_model;
};

} // namespace prev_test::component::sky

#endif // !__SUN_COMPONENT_H__
