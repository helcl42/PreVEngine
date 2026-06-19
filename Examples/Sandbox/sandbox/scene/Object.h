#ifndef __SANDBOX_OBJECT_H__
#define __SANDBOX_OBJECT_H__

#include "../component/ColorComponent.h"
#include "../component/ModelComponent.h"
#include "../component/TransformComponent.h"
#include "../render/Model.h"

#include <prev/scene/graph/SceneNode.h>

#include <glm/glm.hpp>

#include <memory>

namespace sandbox::scene {
class Object final : public prev::scene::graph::SceneNode {
public:
    Object(const std::shared_ptr<sandbox::render::Model>& model, const glm::vec3& position, const glm::vec4& color);

    ~Object() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

private:
    glm::vec3 m_position;
    std::shared_ptr<sandbox::component::TransformComponent> m_transform;
    std::shared_ptr<sandbox::component::ColorComponent> m_color;
    std::shared_ptr<sandbox::component::ModelComponent> m_model;
    float m_angle{ 0.0f };
};
} // namespace sandbox::scene

#endif // !__SANDBOX_OBJECT_H__
