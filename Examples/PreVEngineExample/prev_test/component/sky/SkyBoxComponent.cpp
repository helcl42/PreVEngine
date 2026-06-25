#include "SkyBoxComponent.h"

namespace prev_test::component::sky {
std::shared_ptr<prev_test::render::IModel> SkyBoxComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev_test::render::IMaterial> SkyBoxComponent::GetMaterial() const
{
    return m_material;
}

bool SkyBoxComponent::IsReady() const
{
    return (!m_model || m_model->IsReady()) && (!m_material || m_material->IsReady());
}
} // namespace prev_test::component::sky