#include "CubeRobotPart.h"

#include "../../Tags.h"
#include "../../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../../component/ray_casting/SelectableComponentFactory.h"
#include "../../component/render/RenderComponentFactory.h"
#include "../../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::robot {
CubeRobotPart::CubeRobotPart(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_initialPosition(position)
    , m_initialOrientation(orientation)
    , m_initialScale(scale)
    , m_texturePath(texturePath)
{
}

void CubeRobotPart::Init()
{
    prev_test::component::render::RenderComponentFactory renderComponentFactory{ m_device, m_allocator };
    std::shared_ptr<prev_test::component::render::IRenderComponent> renderComponent = renderComponentFactory.CreateCubeRenderComponent(m_texturePath, true, true);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::render::IRenderComponent>(GetThis(), renderComponent, { TAG_RENDER_COMPONENT });

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{ m_allocator };
    m_boundingVolumeComponent = bondingVolumeFactory.CreateOBB(renderComponent->GetModel()->GetMesh());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, { TAG_BOUNDING_VOLUME_COMPONENT });

    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, { TAG_TRANSFORM_COMPONENT });

    prev_test::component::ray_casting::SelectableComponentFacrory selectableComponentFactory{};
    std::shared_ptr<prev_test::component::ray_casting::ISelectableComponent> selectableComponent = selectableComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::ISelectableComponent>(GetThis(), selectableComponent, { TAG_SELECTABLE_COMPONENT });

    SceneNode::Init();
}

void CubeRobotPart::Update(float deltaTime)
{
    m_transformComponent->Update(deltaTime);
    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void CubeRobotPart::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::robot