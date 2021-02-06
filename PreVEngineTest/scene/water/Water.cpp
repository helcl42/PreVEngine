#include "Water.h"

#include "../../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../../component/transform/TransformComponentFactory.h"
#include "../../component/water/WaterCommon.h"
#include "../../component/water/WaterComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
Water::Water(const int x, const int z)
    : SceneNode({ TAG_WATER_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT, TAG_BOUNDING_VOLUME_COMPONENT })
    , m_x(x)
    , m_z(z)
{
}

void Water::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::water::WaterComponentFactory componentFactory{};
    m_waterComponent = componentFactory.Create(m_x, m_z);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::water::IWaterComponent>(GetThis(), m_waterComponent, TAG_WATER_RENDER_COMPONENT);

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{};
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_waterComponent->GetModel()->GetMesh()->GetVertices());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, TAG_BOUNDING_VOLUME_COMPONENT);

    SceneNode::Init();
}

void Water::Update(float deltaTime)
{
    m_waterComponent->Update(deltaTime);

    m_transformComponent->SetPosition(m_waterComponent->GetPosition());
    m_transformComponent->SetScale(glm::vec3(prev_test::component::water::WATER_TILE_SIZE));

    m_transformComponent->Update(deltaTime);

    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void Water::ShutDown()
{
    SceneNode::ShutDown();
}

} // namespace prev_test::scene::water
