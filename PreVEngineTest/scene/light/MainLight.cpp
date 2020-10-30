#include "MainLight.h"

#include "../../component/light/LightComponentFactory.h"
#include "../../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::scene::light {
MainLight::MainLight(const glm::vec3& pos)
    : SceneNode()
    , m_initialPosition(pos)
{
}

void MainLight::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::light::LightComponentFactory lightFactory{};
    m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::light::ILightComponent>(GetThis(), m_lightComponent, TAG_LIGHT_COMPONENT);

    SceneNode::Init();
}

void MainLight::Update(float deltaTime)
{
    const float ROTATION_SPEED_DEG_PER_SEC = 5.0f;
    const float ROTATION_ANGLE = ROTATION_SPEED_DEG_PER_SEC * deltaTime;

    glm::mat4 transform(1.0f);
    transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::translate(transform, m_lightComponent->GetPosition());

    glm::vec3 position{ transform[3][0], transform[3][1], transform[3][2] };

    m_lightComponent->SetPosition(position);

    auto lightTransformInWorldSpace = glm::inverse(m_lightComponent->LookAt());

    m_transformComponent->SetPosition(prev::util::MathUtil::ExtractTranslation(lightTransformInWorldSpace));
    m_transformComponent->SetOrientation(prev::util::MathUtil::ExtractOrientation(lightTransformInWorldSpace));

    m_transformComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void MainLight::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::light