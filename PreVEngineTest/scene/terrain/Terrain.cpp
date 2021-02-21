#include "Terrain.h"

#include "../../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../../component/terrain/TerrainCommon.h"
#include "../../component/terrain/TerrainComponentFactory.h"
#include "../../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::terrain {
Terrain::Terrain(const int x, const int z)
    : SceneNode()
    , m_xIndex(x)
    , m_zIndex(z)
{
}

void Terrain::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::terrain::TerrainComponentFactory terrainComponentFactory{};
    m_terrainComponent = terrainComponentFactory.CreateRandomTerrainConeStepMapped(m_xIndex, m_zIndex, prev_test::component::terrain::TERRAIN_TILE_SIZE);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::terrain::ITerrainComponenet>(GetThis(), m_terrainComponent, TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT);

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{};
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_terrainComponent->GetModel()->GetMesh());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, TAG_BOUNDING_VOLUME_COMPONENT);

    m_transformComponent->SetPosition(m_terrainComponent->GetPosition());

    m_terrainManagerComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>({ TAG_TERRAIN_MANAGER_COMPONENT });
    if (auto manager = m_terrainManagerComponent.lock()) {
        manager->AddTerrainComponent(m_terrainComponent);
    }

    SceneNode::Init();
}

void Terrain::Update(float deltaTime)
{
    m_transformComponent->Update(deltaTime);
    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransform());

    SceneNode::Update(deltaTime);
}

void Terrain::ShutDown()
{
    SceneNode::ShutDown();

    if (auto manager = m_terrainManagerComponent.lock()) {
        manager->RemoveTerrain(m_terrainComponent);
    }
}
} // namespace prev_test::scene::terrain