#include "TerrainManager.h"
#include "Terrain.h"

#include "../../component/ray_casting/SelectableComponentFactory.h"
#include "../../component/terrain/TerrainManagerComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::terrain {
TerrainManager::TerrainManager(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const uint32_t maxX, const uint32_t maxZ)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_gridMaxX(maxX)
    , m_gridMaxZ(maxZ)
{
}

void TerrainManager::Init()
{
    std::shared_ptr<prev_test::component::terrain::ITerrainManagerComponent> terrainManagerComponent = prev_test::component::terrain::TerrainManagerComponentFactory{}.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::terrain::ITerrainManagerComponent>(GetThis(), terrainManagerComponent, { TAG_TERRAIN_MANAGER_COMPONENT });

    prev_test::component::ray_casting::SelectableComponentFacrory selectableComponentFactory{};
    std::shared_ptr<prev_test::component::ray_casting::ISelectableComponent> selectableComponent = selectableComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::ISelectableComponent>(GetThis(), selectableComponent, { TAG_SELECTABLE_COMPONENT });

    for (uint32_t x = 0; x < m_gridMaxX; x++) {
        for (uint32_t z = 0; z < m_gridMaxZ; z++) {
            auto terrain = std::make_shared<Terrain>(m_device, m_allocator, static_cast<int>(x), static_cast<int>(z));
            AddChild(terrain);
        }
    }

    SceneNode::Init();

    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = -std::numeric_limits<float>::max();
    auto terrains = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::terrain::ITerrainComponent>(GetRoot(), { TAG_TERRAIN_RENDER_COMPONENT, TAG_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT, TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT });
    for (const auto& terrain : terrains) {
        auto heightInfo = terrain->GetHeightMapInfo();
        minHeight = std::min(minHeight, heightInfo->minHeight);
        maxHeight = std::max(maxHeight, heightInfo->maxHeight);
    }

    for (auto& terrain : terrains) {
        auto heightInfo = terrain->GetHeightMapInfo();
        heightInfo->globalMinHeight = minHeight;
        heightInfo->globalMaxHeight = maxHeight;
    }
}

void TerrainManager::Update(float deltaTime)
{
    SceneNode::Update(deltaTime);
}

void TerrainManager::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::terrain