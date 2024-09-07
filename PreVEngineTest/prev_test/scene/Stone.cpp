#include "Stone.h"

#include "../common/AssetManager.h"
#include "../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../component/ray_casting/SelectableComponentFactory.h"
#include "../component/render/RenderComponentFactory.h"
#include "../component/terrain/ITerrainManagerComponent.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
Stone::Stone(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_initialPosition(position)
    , m_initialOrientation(orientation)
    , m_initialScale(scale)
{
}

void Stone::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::render::RenderComponentFactory componentFactory{ m_device, m_allocator };
    std::shared_ptr<prev_test::component::render::IRenderComponent> renderComponent = componentFactory.CreateModelRenderComponent(prev_test::common::AssetManager::Instance().GetAssetPath("Models/Boulder/boulder.fbx"), { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Boulder/boulder.png") }, { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Boulder/boulder_normal.png") }, { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Boulder/boulder_cone.png") }, true, true);
    renderComponent->GetMaterial()->SetHeightScale(0.01f);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::render::IRenderComponent>(GetThis(), renderComponent, TAG_RENDER_CONE_STEP_MAPPED_COMPONENT);

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{ m_allocator };
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(renderComponent->GetModel()->GetMesh());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, TAG_BOUNDING_VOLUME_COMPONENT);

    prev_test::component::ray_casting::SelectableComponentFacrory selectableComponentFactory{};
    std::shared_ptr<prev_test::component::ray_casting::ISelectableComponent> selectableComponent = selectableComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::ISelectableComponent>(GetThis(), selectableComponent, TAG_SELECTABLE_COMPONENT);

    SceneNode::Init();
}

void Stone::Update(float deltaTime)
{
    const auto terrain = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>({ TAG_TERRAIN_MANAGER_COMPONENT });

    auto currentPosition = m_transformComponent->GetPosition();

    float height{ 0.0f };
    terrain->GetHeightAt(currentPosition, height);

    m_transformComponent->SetPosition(glm::vec3(currentPosition.x, height, currentPosition.z));

    m_transformComponent->Update(deltaTime);

    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void Stone::ShutDown()
{
    SceneNode::ShutDown();
}

} // namespace prev_test::scene