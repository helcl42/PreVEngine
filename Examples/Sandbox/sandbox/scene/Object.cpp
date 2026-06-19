#include "Object.h"

#include "../Tags.h"

#include <prev/scene/component/NodeComponentHelper.h>

#include <glm/gtc/matrix_transform.hpp>

namespace sandbox::scene {
Object::Object(const std::shared_ptr<sandbox::render::Model>& model, const glm::vec3& position, const glm::vec4& color)
    : SceneNode({ sandbox::TAG_RENDERABLE })
    , m_position{ position }
{
    // The components own the data; they are registered on the node in Init() (which needs GetThis()).
    m_transform = std::make_shared<sandbox::component::TransformComponent>(glm::translate(glm::mat4{ 1.0f }, m_position));
    m_color = std::make_shared<sandbox::component::ColorComponent>(color);
    m_model = std::make_shared<sandbox::component::ModelComponent>(model);
}

void Object::Init()
{
    prev::scene::component::NodeComponentHelper::AddComponent<sandbox::component::TransformComponent>(GetThis(), m_transform);
    prev::scene::component::NodeComponentHelper::AddComponent<sandbox::component::ColorComponent>(GetThis(), m_color);
    prev::scene::component::NodeComponentHelper::AddComponent<sandbox::component::ModelComponent>(GetThis(), m_model);

    SceneNode::Init();
}

void Object::Update(float deltaTime)
{
    m_angle += deltaTime * 0.8f;
    // Spin in place at the node's own position: translate then rotate about its center.
    m_transform->SetWorldTransform(glm::rotate(glm::translate(glm::mat4{ 1.0f }, m_position), m_angle, glm::vec3{ 0.3f, 1.0f, 0.0f }));

    SceneNode::Update(deltaTime);
}
} // namespace sandbox::scene
