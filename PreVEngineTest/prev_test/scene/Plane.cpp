#include "Plane.h"

#include "../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../component/render/RenderComponentFactory.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
Plane::Plane(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const float heightScale)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_initialPosition(position)
    , m_initialOrientation(orientation)
    , m_initialScale(scale)
    , m_texturePath(texturePath)
    , m_normalMapPath(normalMapPath)
    , m_heightMapPath(heightMapPath)
    , m_heightScale(heightScale)
{
}

void Plane::Init()
{
    prev_test::component::render::RenderComponentFactory renderComponentFactory{ m_device, m_allocator };
    std::shared_ptr<prev_test::component::render::IRenderComponent> renderComponent = renderComponentFactory.CreatePlaneRenderComponent(m_texturePath, m_normalMapPath, m_heightMapPath, false, true);
    renderComponent->GetMaterial()->SetHeightScale(m_heightScale);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::render::IRenderComponent>(GetThis(), renderComponent, { TAG_RENDER_CONE_STEP_MAPPED_COMPONENT });

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{ m_allocator };
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(renderComponent->GetModel()->GetMesh());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, { TAG_BOUNDING_VOLUME_COMPONENT });

    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, { TAG_TRANSFORM_COMPONENT });

    SceneNode::Init();
}

void Plane::Update(float deltaTime)
{
    const float DEGS_PER_SEC = 5.0;
    m_transformComponent->Rotate(glm::quat(glm::radians(glm::vec3(DEGS_PER_SEC * deltaTime, 0.0f, 0.0f))));
    m_transformComponent->Rotate(glm::quat(glm::radians(glm::vec3(0.0f, DEGS_PER_SEC * deltaTime, 0.0f))));

    m_transformComponent->Update(deltaTime);
    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void Plane::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene