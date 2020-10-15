#include "SelectableComponent.h"

namespace prev_test::component::ray_casting {
bool SelectableComponent::IsSelected() const
{
    return m_selected;
}

void SelectableComponent::SetSelected(const bool selected)
{
    m_selected = selected;
}

const glm::vec3& SelectableComponent::GetPostiion() const
{
    return m_position;
}

void SelectableComponent::SetPosition(const glm::vec3& at)
{
    m_position = at;
}

void SelectableComponent::Reset()
{
    m_selected = false;
    m_position = glm::vec3(std::numeric_limits<float>::min());
}
} // namespace prev_test::component::ray_casting