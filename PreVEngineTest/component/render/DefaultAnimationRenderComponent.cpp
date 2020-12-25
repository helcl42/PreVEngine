#include "DefaultAnimationRenderComponent.h"

namespace prev_test::component::render {
DefaultAnimationRenderComponent::DefaultAnimationRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& materials, const std::vector<std::shared_ptr<prev_test::render::IAnimation> >& animations, const bool castsShadows, const bool isCastedByShadows)
    : m_model(model)
    , m_materials(materials)
    , m_animations(animations)
    , m_castsShadows(castsShadows)
    , m_isCastedByShadows(isCastedByShadows)
    , m_currentAnimationIndex(0)
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

bool DefaultAnimationRenderComponent::CastsShadows() const
{
    return m_castsShadows;
}

bool DefaultAnimationRenderComponent::IsCastedByShadows() const
{
    return m_isCastedByShadows;
}

std::shared_ptr<prev_test::render::IAnimation> DefaultAnimationRenderComponent::GetAnimation(const uint32_t index) const
{
    return m_animations.at(index);
}

const std::vector<std::shared_ptr<prev_test::render::IAnimation> >& DefaultAnimationRenderComponent::GetAnimations() const
{
    return m_animations;
}

std::shared_ptr<prev_test::render::IAnimation> DefaultAnimationRenderComponent::GetCurrentAnimation() const
{
    return m_animations.at(m_currentAnimationIndex);
}

void DefaultAnimationRenderComponent::SetCurrentAnimationIndex(const uint32_t index)
{
    m_currentAnimationIndex = index;
}
} // namespace prev_test::component::render