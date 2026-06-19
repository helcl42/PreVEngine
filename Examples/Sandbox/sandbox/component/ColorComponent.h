#ifndef __SANDBOX_COLOR_COMPONENT_H__
#define __SANDBOX_COLOR_COMPONENT_H__

#include <prev/scene/component/IComponent.h>

#include <glm/glm.hpp>

namespace sandbox::component {
// Per-sandbox base color (rgb used; a kept for completeness).
class ColorComponent final : public prev::scene::component::IComponent {
public:
    explicit ColorComponent(const glm::vec4& color = glm::vec4{ 1.0f })
        : m_color{ color }
    {
    }

    const glm::vec4& GetColor() const
    {
        return m_color;
    }

private:
    glm::vec4 m_color{ 1.0f };
};
} // namespace sandbox::component

#endif // !__SANDBOX_COLOR_COMPONENT_H__
