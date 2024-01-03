#include "SkyComponent.h"

namespace prev_test::component::sky {
std::shared_ptr<prev_test::render::IModel> SkyComponent::GetModel() const
{
    return m_model;
}

glm::vec3 SkyComponent::GetBottomColor() const
{
    return m_bottomColor;
}

glm::vec3 SkyComponent::GetTopColor() const
{
    return m_topColor;
}
} // namespace prev_test::component::sky