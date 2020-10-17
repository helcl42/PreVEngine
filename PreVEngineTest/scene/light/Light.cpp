#include "Light.h"
#include "../../component/light/LightComponentFactory.h"
#include "../../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::light {
Light::Light(const glm::vec3& position, const glm::vec3& color)
    : SceneNode()
    , m_initialPosition(position)
    , m_color(color)
{
}

void Light::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<SceneNodeFlags, prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<SceneNodeFlags, prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

    prev_test::component::light::LightComponentFactory lightFactory{};
    m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition, m_color, glm::vec3(0.1f, 0.005f, 0.001f));
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::light::ILightComponent>(GetThis(), m_lightComponent, SceneNodeFlags::LIGHT_COMPONENT);

    SceneNode::Init();
}

void Light::Update(float deltaTime)
{
    m_transformComponent->SetPosition(m_lightComponent->GetPosition());
    m_transformComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void Light::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::light