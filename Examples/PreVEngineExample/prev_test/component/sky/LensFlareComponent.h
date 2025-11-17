#ifndef __LENS_FLARE_COMPONENT_H__
#define __LENS_FLARE_COMPONENT_H__

#include "ILensFlareComponent.h"

namespace prev_test::component::sky {
class LensFlareComponent : public ILensFlareComponent {
public:
    explicit LensFlareComponent(const std::vector<Flare>& flares, const float spacing, const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& materials, const std::shared_ptr<prev_test::render::IModel>& model);

    virtual ~LensFlareComponent() = default;

public:
    const std::vector<Flare>& GetFlares() const override;

    const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& GetMaterials() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::vector<glm::vec2> ComputeFlarePositions(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const override;

private:
    std::vector<Flare> m_flares;

    float m_spacing;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> m_materials;

    std::shared_ptr<prev_test::render::IModel> m_model;
};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_H__
