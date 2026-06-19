#ifndef __SANDBOX_TRANSFORM_COMPONENT_H__
#define __SANDBOX_TRANSFORM_COMPONENT_H__

#include <prev/scene/component/IComponent.h>

#include <glm/glm.hpp>

namespace sandbox::component {
// Minimal transform: just the world (model) matrix used to place a renderable in the scene.
class TransformComponent final : public prev::scene::component::IComponent {
public:
    explicit TransformComponent(const glm::mat4& worldTransform = glm::mat4{ 1.0f })
        : m_worldTransform{ worldTransform }
    {
    }

    const glm::mat4& GetWorldTransform() const
    {
        return m_worldTransform;
    }

    void SetWorldTransform(const glm::mat4& worldTransform)
    {
        m_worldTransform = worldTransform;
    }

private:
    glm::mat4 m_worldTransform{ 1.0f };
};
} // namespace sandbox::component

#endif // !__SANDBOX_TRANSFORM_COMPONENT_H__
