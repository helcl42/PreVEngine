#include "DefaultRenderComponent.h"

namespace prev_test::component::render {
DefaultRenderComponent::DefaultRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& materials, const bool castsShadows, const bool isCastedByShadows)
    : m_model(model)
    , m_materials(materials)
    , m_castsShadows(castsShadows)
    , m_isCastedByShadows(isCastedByShadows)
{
}

DefaultRenderComponent::DefaultRenderComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::shared_ptr<prev_test::render::IMaterial>& material, const bool castsShadows, const bool isCastedByShadows)
    : DefaultRenderComponent(model, std::vector<std::shared_ptr<prev_test::render::IMaterial>>{ material }, castsShadows, isCastedByShadows)
{
}

std::shared_ptr<prev_test::render::IModel> DefaultRenderComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev_test::render::IMaterial> DefaultRenderComponent::GetMaterial(const uint32_t index) const
{
    return m_materials[index];
}

const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& DefaultRenderComponent::GetMaterials() const
{
    return m_materials;
}

bool DefaultRenderComponent::CastsShadows() const
{
    return m_castsShadows;
}

bool DefaultRenderComponent::IsCastedByShadows() const
{
    return m_isCastedByShadows;
}
} // namespace prev_test::component::render