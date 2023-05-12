#ifndef __DEFAULT_RENDER_COMPONENT_H__
#define __DEFAULT_RENDER_COMPONENT_H__

#include "IRenderComponent.h"

namespace prev_test::component::render {
class DefaultRenderComponent : public IRenderComponent {
public:
    DefaultRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& materials, const bool castsShadows, const bool isCastedByShadows);

    DefaultRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::shared_ptr<prev_test::render::IMaterial>& material, const bool castsShadows, const bool isCastedByShadows);

    virtual ~DefaultRenderComponent() = default;

public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial(const uint32_t index = 0) const override;

    const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& GetMaterials() const override;

    bool CastsShadows() const override;

    bool IsCastedByShadows() const override;

private:
    std::shared_ptr<prev_test::render::IModel> m_model;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> m_materials;

    bool m_castsShadows;

    bool m_isCastedByShadows;
};
} // namespace prev_test::component::render

#endif // !__DEFAULT_RENDER_COMPONENT_H__
