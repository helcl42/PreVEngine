#include "DefaultAnimationRenderComponent.h"

namespace prev_test::component::render {
DefaultAnimationRenderComponent::DefaultAnimationRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& materials, const std::vector<std::shared_ptr<prev_test::render::IAnimation> >& animations, const bool castsShadows, const bool isCastedByShadows)
    : m_model(model)
    , m_materials(materials)
    , m_animations(animations)
    , m_castsShadows(castsShadows)
    , m_isCastedByShadows(isCastedByShadows)
{
}

std::shared_ptr<prev_test::render::IModel> DefaultAnimationRenderComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev_test::render::IMaterial> DefaultAnimationRenderComponent::GetMaterial(const uint32_t index) const
{
    return m_materials.at(index);
}

const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& DefaultAnimationRenderComponent::GetMaterials() const
{
    return m_materials;
}

std::shared_ptr<prev_test::render::IAnimation> DefaultAnimationRenderComponent::GetAnimation(const uint32_t index) const
{
    return m_animations.at(index);
}

const std::vector<std::shared_ptr<prev_test::render::IAnimation> >& DefaultAnimationRenderComponent::GetAnimations() const
{
    return m_animations;
}

bool DefaultAnimationRenderComponent::CastsShadows() const
{
    return m_castsShadows;
}

bool DefaultAnimationRenderComponent::IsCastedByShadows() const
{
    return m_isCastedByShadows;
}
} // namespace prev_test::component::render