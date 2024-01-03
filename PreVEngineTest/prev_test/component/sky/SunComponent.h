#ifndef __SUN_COMPONENT_H__
#define __SUN_COMPONENT_H__

#include "ISunComponent.h"

namespace prev_test::component::sky {
class SunComponent : public ISunComponent {
public:
    explicit SunComponent(const std::shared_ptr<Flare>& flare, const std::shared_ptr<prev_test::render::IModel>& model);

    virtual ~SunComponent() = default;

public:
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override;

    std::shared_ptr<Flare> GetFlare() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition);

private:
    std::shared_ptr<Flare> m_flare;

    std::shared_ptr<prev_test::render::IModel> m_model;
};

} // namespace prev_test::component::sky

#endif // !__SUN_COMPONENT_H__
