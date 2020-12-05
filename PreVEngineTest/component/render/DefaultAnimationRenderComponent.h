#ifndef __DEFAULT_ANIMATION_RENDER_COMPONENT_H__
#define __DEFAULT_ANIMATION_RENDER_COMPONENT_H__

#include "IAnimationRenderComponent.h"

namespace prev_test::component::render {
class DefaultAnimationRenderComponent : public IAnimationRenderComponent {
public:
    DefaultAnimationRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& materials, const std::vector<std::shared_ptr<prev_test::render::IAnimation>>& animations, const bool castsShadows, const bool isCastedByShadows);

    virtual ~DefaultAnimationRenderComponent() = default;

public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial(const uint32_t index = 0) const override;

    const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& GetMaterials() const override;

    std::shared_ptr<prev_test::render::IAnimation> GetAnimation(const uint32_t index = 0) const override;

    const std::vector<std::shared_ptr<prev_test::render::IAnimation> >& GetAnimations() const override;

    bool CastsShadows() const override;

    bool IsCastedByShadows() const override;

private:
    std::shared_ptr<prev_test::render::IModel> m_model;

    std::vector<std::shared_ptr<prev_test::render::IMaterial> > m_materials;

    std::vector<std::shared_ptr<prev_test::render::IAnimation>> m_animations;

    bool m_castsShadows;

    bool m_isCastedByShadows;
};
} // namespace prev_test::component::render

#endif