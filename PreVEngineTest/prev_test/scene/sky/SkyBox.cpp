#include "SkyBox.h"

#include "../../component/camera/ICameraComponent.h"
#include "../../component/sky/SkyBoxComponentFactory.h"
#include "../../component/sky/SkyCommon.h"
#include "../../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
SkyBox::SkyBox()
    : SceneNode()
{
}

void SkyBox::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::sky::SkyBoxComponentFactory factory{};
    m_skyBoxComponent = factory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ISkyBoxComponent>(GetThis(), m_skyBoxComponent, TAG_SKYBOX_RENDER_COMPONENT);

    SceneNode::Init();
}

void SkyBox::Update(float deltaTime)
{
    auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>(prev::common::TagSet{ TAG_MAIN_CAMERA });

    const float ROTATION_ANGLE = ROTATION_SPEED_DEGS_PER_SEC * deltaTime;

    glm::mat4 transform(1.0f);
    transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));

    m_transformComponent->Rotate(glm::quat_cast(transform));
    m_transformComponent->SetPosition(cameraComponent->GetPosition());
    m_transformComponent->SetScale(glm::vec3(prev_test::component::sky::SKY_BOX_SIZE));

    m_transformComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void SkyBox::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::sky
