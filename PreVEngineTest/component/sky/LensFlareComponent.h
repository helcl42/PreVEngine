#ifndef __LENS_FLARE_COMPONENT_H__
#define __LENS_FLARE_COMPONENT_H__

#include "ILensFlareComponent.h"

namespace prev_test::component::sky {
class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<std::shared_ptr<Flare>>& flares, float spacing, const std::shared_ptr<prev_test::render::IModel>& model);

    virtual ~LensFlareComponent() = default;

public:
    void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) override;

    const std::vector<std::shared_ptr<Flare>>& GetFlares() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

private:
    bool ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition);

    void UpdateFlareTexrures(const glm::vec2& sunToCenterDirection, const glm::vec2& sunPositionInScreenSpace);

private:
    std::vector<std::shared_ptr<Flare>> m_flares;

    float m_spacing;

    std::shared_ptr<prev_test::render::IModel> m_model;
};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_H__
